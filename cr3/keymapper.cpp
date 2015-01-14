#include "keymapper.h"

KeyMapper::KeyMapper()
{
    keyMap.insert( Qt::Key_Q, "Q" );
    keyMap.insert( Qt::Key_W, "W" );
    keyMap.insert( Qt::Key_E, "E" );
    keyMap.insert( Qt::Key_R, "R" );
    keyMap.insert( Qt::Key_T, "T" );
    keyMap.insert( Qt::Key_Y, "Y" );
    keyMap.insert( Qt::Key_U, "U" );
    keyMap.insert( Qt::Key_I, "I" );
    keyMap.insert( Qt::Key_O, "O" );
    keyMap.insert( Qt::Key_P, "P" );
    keyMap.insert( Qt::Key_A, "A" );
    keyMap.insert( Qt::Key_S, "S" );
    keyMap.insert( Qt::Key_D, "D" );
    keyMap.insert( Qt::Key_F, "F" );
    keyMap.insert( Qt::Key_G, "G" );
    keyMap.insert( Qt::Key_H, "H" );
    keyMap.insert( Qt::Key_J, "J" );
    keyMap.insert( Qt::Key_K, "K" );
    keyMap.insert( Qt::Key_L, "L" );
    keyMap.insert( Qt::Key_Z, "Z" );
    keyMap.insert( Qt::Key_X, "X" );
    keyMap.insert( Qt::Key_C, "C" );
    keyMap.insert( Qt::Key_V, "V" );
    keyMap.insert( Qt::Key_B, "B" );
    keyMap.insert( Qt::Key_N, "N" );
    keyMap.insert( Qt::Key_M, "M" );
    keyMap.insert( Qt::Key_Space, "Space" );
    keyMap.insert( Qt::Key_Menu, "Menu" );
    keyMap.insert( Qt::Key_Up, "Up" );
    keyMap.insert( Qt::Key_Down, "Down" );
    keyMap.insert( Qt::Key_Left, "Left" );
    keyMap.insert( Qt::Key_Right, "Right" );
    keyMap.insert( Qt::Key_Select, "Select" );
    keyMap.insert( Qt::Key_PageUp, "PageUp" );
    keyMap.insert( Qt::Key_PageDown, "PageDown" );
    keyMap.insert( Qt::Key_Home, "Home" );
    keyMap.insert( Qt::Key_Back, "Back" );
    keyMap.insert( Qt::Key_Return, "Return" );
}

QString KeyMapper::map(int code) const
{
    QMap<int, QString>::iterator i(keyMap.find(code));
    return i != keyMap.end()? *i : "";
}
