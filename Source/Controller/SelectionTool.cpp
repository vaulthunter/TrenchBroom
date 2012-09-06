/*
 Copyright (C) 2010-2012 Kristian Duske
 
 This file is part of TrenchBroom.
 
 TrenchBroom is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 TrenchBroom is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with TrenchBroom.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "SelectionTool.h"

#include "Controller/ChangeEditStateCommand.h"
#include "Model/Brush.h"
#include "Model/EditStateManager.h"
#include "Model/Entity.h"
#include "Model/Face.h"
#include "Model/Picker.h"
#include "Utility/List.h"

namespace TrenchBroom {
    namespace Controller {
        bool SelectionTool::handleMouseUp(InputEvent& event) {
            if (event.mouseButtons != MouseButtons::Left)
                return false;
            
            Model::Hit* hit = event.pickResult->first(Model::Hit::EntityHit | Model::Hit::FaceHit, false);
            Command* command = NULL;
            Model::EditStateManager& editStateManager = document().EditStateManager();
            
            if (hit != NULL) {
                bool multi = event.modifierKeys == ModifierKeys::CtrlCmd;
                
                if (hit->type() == Model::Hit::EntityHit) {
                    Model::Entity& entity = hit->entity();
                    if (multi) {
                        if (entity.selected())
                            command = ChangeEditStateCommand::deselect(document(), entity);
                        else
                            command = ChangeEditStateCommand::select(document(), entity);
                    } else {
                        command = ChangeEditStateCommand::replace(document(), entity);
                    }
                } else {
                    Model::Face& face = hit->face();
                    Model::Brush& brush = *face.brush();
                    
                    if (brush.selected()) {
                        if (multi)
                            command = ChangeEditStateCommand::deselect(document(), brush);
                        else
                            command = ChangeEditStateCommand::select(document(), face);
                    } else if (face.selected()) {
                        if (multi)
                            command = ChangeEditStateCommand::deselect(document(), face);
                        else
                            command = ChangeEditStateCommand::select(document(), brush);
                    } else {
                        if (multi) {
                            if (editStateManager.selectionMode() == Model::EditStateManager::Faces)
                                command = ChangeEditStateCommand::select(document(), face);
                            else
                                command = ChangeEditStateCommand::select(document(), brush);
                        } else {
                            if (editStateManager.selectionMode() == Model::EditStateManager::Faces)
                                command = ChangeEditStateCommand::replace(document(), face);
                            else
                                command = ChangeEditStateCommand::replace(document(), brush);
                        }
                    }
                }
            } else {
                command = ChangeEditStateCommand::deselectAll(document());
            }

            if (command != NULL) {
                postCommand(command);
                return true;
            }
            
            return false;
        }
        
        bool SelectionTool::handleScrolled(InputEvent& event) {
            if (event.modifierKeys != ModifierKeys::CtrlCmd)
                return false;
            
            Model::EditStateManager& editStateManager = document().EditStateManager();
            if (editStateManager.selectionMode() == Model::EditStateManager::None ||
                editStateManager.selectionMode() == Model::EditStateManager::Faces)
                return false;
            
            Model::HitList hits = event.pickResult->hits(Model::Hit::EntityHit | Model::Hit::FaceHit);
            if (hits.empty())
                return false;
            
            Model::EntityList entities = editStateManager.selectedEntities();
            Model::BrushList brushes = editStateManager.selectedBrushes();
            
            Command* command = NULL;
            bool selectNext = false;
            for (unsigned int i = 0; i < hits.size() && !selectNext; i++) {
                Model::Hit* hit = hits[i];
                if (hit->type() == Model::Hit::EntityHit) {
                    Model::Entity& entity = hit->entity();
                    if (entity.selected()) {
                        Utility::erase(entities, &entity);
                        selectNext = true;
                    }
                } else {
                    Model::Face& face = hit->face();
                    Model::Brush& brush = *face.brush();
                    if (brush.selected()) {
                        Utility::erase(brushes, &brush);
                        selectNext = true;
                    }
                }
                
                if (selectNext) {
                    Model::Hit* hit = NULL;
                    if (event.scrollY > 0.0f && i < hits.size() - 1)
                        hit = hits[i + 1];
                    else if (event.scrollY < 0.0f && i > 0)
                        hit = hits[i - 1];
                    
                    if (hit != NULL) {
                        if (hit->type() == Model::Hit::EntityHit) {
                            Model::Entity& entity = hit->entity();
                            if (!entity.selected()) {
                                entities.push_back(&entity);
                                command = ChangeEditStateCommand::replace(document(), entities, brushes);
                            }
                        } else {
                            Model::Face& face = hit->face();
                            Model::Brush& brush = *face.brush();
                            if (!brush.selected()) {
                                brushes.push_back(&brush);
                                command = ChangeEditStateCommand::replace(document(), entities, brushes);
                            }
                        }
                    }
                }
            }
            
            if (command != NULL) {
                postCommand(command);
                return true;
            }
            
            return false;
        }
    }
}