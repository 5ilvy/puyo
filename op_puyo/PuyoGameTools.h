#ifndef PUYO_GAME_TOOLS_H
#define PUYO_GAME_TOOLS_H

#include <iostream>
//ice以外の環境では-std=c++03をつけてコンパイル

const class {
public:
    template <class T>    // あらゆる型のポインタに
    operator T*() const {   // 変換できる
        return 0;         // ゼロを返す
    }

    template <class C, class T>  // あらゆる型のメンバのポインタに
    operator T C::*() const {    // 変換できる
        return 0;                // こちらもゼロ
    }

private:
    void operator&()  const; // アドレスは取得できない
} nullPtr;

#endif
