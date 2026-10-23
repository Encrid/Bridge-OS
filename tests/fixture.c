/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#if defined(_WIN32)
__declspec(dllexport)
#endif
int fixture_answer(void)
{
    return (42);
}
