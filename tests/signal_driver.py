# Copyright (C) 2026 Enkrid
#
# Under the GNU Affero General Public License, version 3 only: see license.
import ctypes
import os
import signal
import subprocess
import sys
import threading
import time

SCENARIOS = {
    'interrupt': {'posix': ['SIGINT'], 'windows': ['ctrl-c']},
    'interrupt twice': {'posix': ['SIGINT', 'SIGINT'], 'windows': ['ctrl-c', 'ctrl-c']},
    'termination': {'posix': ['SIGTERM'], 'windows': ['ctrl-break']},
    'termination twice': {'posix': ['SIGTERM', 'SIGTERM'], 'windows': ['ctrl-break', 'ctrl-break']},
    'hang-up': {'posix': ['SIGHUP'], 'windows': ['close']},
    'interrupt, hang-up': {'posix': ['SIGINT', 'SIGHUP'], 'windows': ['ctrl-c', 'close']},
    'ignored only': {'ignored': True, 'posix': ['SIGHUP'], 'windows': ['ctrl-c']},
    'ignored, then caught': {'ignored': True, 'posix': ['SIGHUP', 'SIGINT'], 'windows': ['ctrl-c', 'close']},
    'caught, then caught': {'posix': ['SIGHUP', 'SIGINT'], 'windows': ['ctrl-c', 'close']},
    'closed output': {'output': 'closed', 'posix': ['SIGINT'], 'windows': ['ctrl-c']},
    'closed output, twice': {'output': 'closed', 'posix': ['SIGINT', 'SIGINT'], 'windows': ['ctrl-c', 'ctrl-c']},
    'full output': {'output': 'full', 'posix': ['SIGINT'], 'windows': ['ctrl-c']},
    'full output, twice': {'output': 'full', 'posix': ['SIGINT', 'SIGINT'], 'windows': ['ctrl-c', 'ctrl-c']},
}
WAIT = 20
PAUSE = 1


class Posix:
    def __init__(self, command, steps, ignored):
        for name in ('SIGINT', 'SIGTERM', 'SIGHUP'):
            kept = signal.SIG_IGN if ignored and name == steps[0] else signal.SIG_DFL
            signal.signal(getattr(signal, name), kept)
        self.child = subprocess.Popen(command, stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                                      stderr=subprocess.STDOUT)
        self.out, self.into = self.child.stdout, self.child.stdin

    def act(self, step):
        self.child.send_signal(getattr(signal, step))

    def ended(self):
        try:
            return self.child.wait(WAIT)
        except subprocess.TimeoutExpired:
            self.child.kill()
            return None


class Windows:
    def __init__(self, command, steps, ignored):
        import msvcrt
        from ctypes import wintypes as w
        self.k = ctypes.WinDLL('kernel32', use_last_error=True)
        self.w, self.closing = w, None

        class Coord(ctypes.Structure):
            _fields_ = [('X', w.SHORT), ('Y', w.SHORT)]

        class Start(ctypes.Structure):
            _fields_ = [('cb', w.DWORD), ('reserved', w.LPWSTR), ('desktop', w.LPWSTR), ('title', w.LPWSTR)]
            _fields_ += [(name, w.DWORD) for name in ('x', 'y', 'width', 'height', 'columns', 'rows', 'fill', 'flags')]
            _fields_ += [('show', w.WORD), ('reserved2', w.WORD), ('reserved3', ctypes.c_void_p),
                         ('input', w.HANDLE), ('output', w.HANDLE), ('errors', w.HANDLE), ('attributes', ctypes.c_void_p)]

        class Process(ctypes.Structure):
            _fields_ = [('process', w.HANDLE), ('thread', w.HANDLE), ('pid', w.DWORD), ('tid', w.DWORD)]

        self.k.CreatePseudoConsole.argtypes = [Coord, w.HANDLE, w.HANDLE, w.DWORD, ctypes.POINTER(w.HANDLE)]
        self.k.ClosePseudoConsole.argtypes = [w.HANDLE]
        self.k.ClosePseudoConsole.restype = None
        self.k.UpdateProcThreadAttribute.argtypes = [ctypes.c_void_p, w.DWORD, ctypes.c_size_t, ctypes.c_void_p,
                                                     ctypes.c_size_t, ctypes.c_void_p, ctypes.c_void_p]
        self.k.CreateProcessW.argtypes = [w.LPCWSTR, w.LPWSTR, ctypes.c_void_p, ctypes.c_void_p, w.BOOL, w.DWORD,
                                          ctypes.c_void_p, w.LPCWSTR, ctypes.c_void_p, ctypes.c_void_p]
        tty_read, tty_write = self.pipe()
        screen_read, screen_write = self.pipe()
        self.console = w.HANDLE()
        self.check(self.k.CreatePseudoConsole(Coord(80, 25), tty_read, screen_write, 0,
                                              ctypes.byref(self.console)) == 0, 'CreatePseudoConsole')
        in_read, in_write = self.pipe()
        out_read, out_write = self.pipe()
        for handle in (in_read, out_write):
            self.k.SetHandleInformation(handle, 1, 1)
        size = ctypes.c_size_t()
        self.k.InitializeProcThreadAttributeList(None, 1, 0, ctypes.byref(size))
        self.attributes = ctypes.create_string_buffer(size.value)
        self.check(self.k.InitializeProcThreadAttributeList(self.attributes, 1, 0, ctypes.byref(size)), 'attributes')
        self.check(self.k.UpdateProcThreadAttribute(self.attributes, 0, 0x00020016, self.console,
                                                    ctypes.sizeof(w.HANDLE), None, None), 'pseudoconsole')
        start = Start(cb=ctypes.sizeof(Start), flags=0x100, input=in_read, output=out_write, errors=out_write,
                      attributes=ctypes.cast(self.attributes, ctypes.c_void_p))
        self.info = Process()
        group = 0x200 if 'ctrl-break' in steps else 0
        self.k.SetConsoleCtrlHandler(None, ignored)
        line = ctypes.create_unicode_buffer(subprocess.list2cmdline(command))
        self.check(self.k.CreateProcessW(None, line, None, None, True, 0x80000 | group, None, None,
                                         ctypes.byref(start), ctypes.byref(self.info)), 'CreateProcessW')
        for handle in (tty_read, screen_write, in_read, out_write):
            self.k.CloseHandle(handle)
        screen = msvcrt.open_osfhandle(screen_read.value, 0)
        threading.Thread(target=drain, args=(os.fdopen(screen, 'rb'),), daemon=True).start()
        self.tty = os.fdopen(msvcrt.open_osfhandle(tty_write.value, 0), 'wb', buffering=0)
        self.out = os.fdopen(msvcrt.open_osfhandle(out_read.value, 0), 'rb')
        self.into = os.fdopen(msvcrt.open_osfhandle(in_write.value, 0), 'wb')

    def pipe(self):
        read, write = self.w.HANDLE(), self.w.HANDLE()
        self.check(self.k.CreatePipe(ctypes.byref(read), ctypes.byref(write), None, 0), 'CreatePipe')
        return read, write

    def check(self, done, what):
        if not done:
            sys.exit('%s failed: error %d' % (what, ctypes.get_last_error()))

    def act(self, step):
        if step == 'ctrl-c':
            self.tty.write(b'\x03')
        elif step == 'ctrl-break':
            self.k.FreeConsole()
            self.check(self.k.AttachConsole(self.info.pid), 'AttachConsole')
            self.check(self.k.GenerateConsoleCtrlEvent(1, self.info.pid), 'GenerateConsoleCtrlEvent')
            self.k.FreeConsole()
        else:
            self.close()

    def close(self):
        if not self.closing:
            self.closing = threading.Thread(target=self.k.ClosePseudoConsole, args=(self.console,), daemon=True)
            self.closing.start()

    def ended(self):
        code = self.w.DWORD()
        late = self.k.WaitForSingleObject(self.info.process, WAIT * 1000) != 0
        if late:
            self.k.TerminateProcess(self.info.process, 3)
        self.k.GetExitCodeProcess(self.info.process, ctypes.byref(code))
        self.close()
        return None if late else code.value


def drain(out):
    while out.read(1 << 16):
        pass


def relay(out, seen, stop):
    for line in iter(out.readline, b''):
        said = line.decode('utf-8', 'replace').strip()
        print(said, flush=True)
        if said in seen:
            seen[said].set()
        if said == 'ready' and stop:
            return


def main():
    program, scenario, line = sys.argv[1], SCENARIOS[sys.argv[2]], sys.argv[3]
    steps = scenario['windows' if os.name == 'nt' else 'posix']
    output = scenario.get('output', 'read')
    command = [program, line] + (['fill'] if output == 'full' else [])
    child = (Windows if os.name == 'nt' else Posix)(command, steps, scenario.get('ignored', False))
    seen = {'ready': threading.Event(), line: threading.Event()}
    reader = threading.Thread(target=relay, args=(child.out, seen, output != 'read'))
    reader.start()
    if seen['ready'].wait(WAIT) and output != 'read':
        reader.join()
        if output == 'closed':
            child.out.close()
        print('%s the output of the program' % ('closed' if output == 'closed' else 'stopped reading'), flush=True)
        time.sleep(PAUSE)
    for step in steps:
        before = seen[line].is_set()
        child.act(step)
        if before or output != 'read':
            time.sleep(PAUSE)
        else:
            seen[line].wait(PAUSE)
    try:
        child.into.write(b'\n\n')
        child.into.close()
    except OSError:
        pass
    if output == 'full':
        threading.Thread(target=drain, args=(child.out,), daemon=True).start()
    status = child.ended()
    reader.join(WAIT)
    if status is None:
        print('the program did not end in %d seconds' % WAIT, flush=True)
    sys.exit(3 if status is None else status)


if __name__ == '__main__':
    main()
