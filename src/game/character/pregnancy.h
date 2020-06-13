#ifndef WM_PREGNANCY_H
#define WM_PREGNANCY_H

class sGirl;
class ICharacter;

int fertility(const sGirl& girl);
void create_pregnancy(sGirl& girl, int num_children, int type, const ICharacter& father);

#endif //WM_PREGNANCY_H
