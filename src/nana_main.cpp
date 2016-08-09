#include "nana_main.h"

#include <nana/gui/wvl.hpp>
#include <nana/gui/widgets/button.hpp>

#include <iostream>

std::string nana_fun()
{
    using namespace nana;
    using namespace std;
    form fm;
    fm.show();
    fm.events().move([]{cout << "fm se movio" << endl;});
    fm.events().click([&]{fm.hide();});
    exec();
    cout << "bye" << endl;
}
