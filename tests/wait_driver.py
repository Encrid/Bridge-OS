# Copyright (C) 2026 Enkrid
#
# Under the GNU Affero General Public License, version 3 only: see license.
import subprocess
import sys


def main():
    program, nanos, back, runs, extra = sys.argv[1], sys.argv[2], sys.argv[3], int(sys.argv[4]), sys.argv[5:]
    short = 0
    for _ in range(runs):
        said = subprocess.run([program, nanos, back, '1'] + extra, stdout=subprocess.PIPE, text=True).stdout
        short += int(said.split()[0])
    print('%d of %d first waits of a process took less than %s ns' % (short, runs, nanos))


if __name__ == '__main__':
    main()
