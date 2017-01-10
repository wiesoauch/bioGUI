/*
 *  bioGUI - a new universal GUI for bioinformatic applications
 *  Copyright (C) 2016  Markus Joppich
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef BIOGUI_WINDOWWIDGETNODE_H
#define BIOGUI_WINDOWWIDGETNODE_H

#include <QWidget>
#include "WindowNode.h"

class WindowWidgetNode : public WindowNode<QWidget> {
public:

    WindowWidgetNode(WindowComponentFactory* pFactory)
    : WindowNode<QWidget>(pFactory)
    {
    }

    virtual ~WindowWidgetNode()
    {

    }

    virtual CreatedElement getWindowElement( QDomElement* pDOMElement ) = 0;

protected:

    virtual void addNodeAttributes(std::vector<std::string>& vAttributes) = 0;

};


#endif //BIOGUI_WINDOWWIDGETNODE_H
