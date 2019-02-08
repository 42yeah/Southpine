//
//  bus.cpp
//  Southpine
//
//  Created by 周昊 on 2019/2/2.
//  Copyright © 2019 aiofwa. All rights reserved.
//

#include "bus.hpp"


struct bus singleton;


struct bus &bus() {
    return singleton;
}
