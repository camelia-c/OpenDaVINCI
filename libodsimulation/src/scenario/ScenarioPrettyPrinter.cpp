/**
 * hesperia - Simulation environment
 * Copyright (C) 2008 - 2015 Christian Berger, Bernhard Rumpe
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <iostream>
#include <string>

#include "core/data/SerializableData.h"
#include "hesperia/data/scenario/ScenarioNode.h"
#include "hesperia/scenario/ScenarioPrettyPrinter.h"

namespace hesperia {
    namespace scenario {

        using namespace std;
        using namespace core::data;
        using namespace data::scenario;

        ScenarioPrettyPrinter::ScenarioPrettyPrinter() {}

        ScenarioPrettyPrinter::~ScenarioPrettyPrinter() {}

        void ScenarioPrettyPrinter::visit(ScenarioNode &node) {
            try {
                SerializableData &s = dynamic_cast<SerializableData&>(node);
                clog << s.toString() << endl;
            } catch (...) {}
        }

    }
} // hesperia::scenario