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

#ifndef BIOGUI_EXECUTIONADDNODE_H
#define BIOGUI_EXECUTIONADDNODE_H


#include "ExecutionNode.h"

class ExecutionAddNode : public ExecutionNode {
public:

    ExecutionAddNode(QDomElement* pElement)
    : ExecutionNode(pElement)
    {

        m_sSeperator = this->getDomElementAttribute(pElement, "sep", " ").toStdString();



    }

    virtual ~ExecutionAddNode()
    {

    }

    virtual std::vector<std::string> inputs()
    {
        std::vector<std::string> vBase;

        for (int i = 0; i < m_vChildren.size(); ++i)
        {

            ExecutionNode* pChild = m_vChildren.at(i);

            std::vector<std::string> vChildOuts = pChild->outputs();

            vBase.insert(vBase.end(), vChildOuts.begin(), vChildOuts.end());

        }

        return vBase;
    }


    std::string evaluate( std::map< std::string, ExecutionNode*>* pID2Node,
                          std::map<std::string, std::string>* pInputID2Value,
                          std::map<std::string, WidgetFunctionNode*>* pInputID2FunctionWidget)
    {


        std::string sReturn = "";

        for (size_t i = 0; i < m_vChildren.size(); ++i)
        {

            std::string sNewVal = m_vChildren.at(i)->evaluate(pID2Node, pInputID2Value, pInputID2FunctionWidget);

            if ((sReturn.size() > 0) && (sNewVal.size() > 0))
            {
                sReturn += m_sSeperator;
            }

            if (sNewVal.size() > 0)
            {
                sReturn += sNewVal;
            }

        }

        return sReturn;

    }


protected:

    void addNodeAttributes(std::vector<std::string>& vAttributes)
    {
        vAttributes.push_back("sep");
    }


};


#endif //BIOGUI_EXECUTIONADDNODE_H
