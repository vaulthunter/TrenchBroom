/*
 Copyright (C) 2010-2017 Kristian Duske
 
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
 along with TrenchBroom. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TrenchBroom_ShearObjectsTool
#define TrenchBroom_ShearObjectsTool

#include "TrenchBroom.h"
#include "VecMath.h"
#include "Model/Hit.h"
#include "Model/ModelTypes.h"
#include "View/Tool.h"
#include "View/ScaleObjectsTool.h"
#include "View/RotateObjectsHandle.h"
#include "BBox.h"

namespace TrenchBroom {
    namespace Model {
        class PickResult;
    }
    
    namespace Renderer {
        class Camera;
    }
    
    namespace View {
        class ShearObjectsTool : public Tool {
        public:
            static const Model::Hit::HitType ShearToolSideHit;

        private:
            bool m_resizing;

            bool m_constrainVertical;

        // moved from controller

            BBox3 m_bboxAtDragStart;
            Model::Hit m_dragStartHit;
            MapDocumentWPtr m_document;
            Vec3 m_dragCumulativeDelta;

        public:

            bool constrainVertical() const;
            void setConstrainVertical(bool constrainVertical);

        public:
            explicit ShearObjectsTool(MapDocumentWPtr document);
            ~ShearObjectsTool() override;
            
            bool applies() const;

            void pickBackSides(const Ray3& pickRay, const Renderer::Camera& camera, Model::PickResult& pickResult);
            void pick2D(const Ray3& pickRay, const Renderer::Camera& camera, Model::PickResult& pickResult);
            void pick3D(const Ray3& pickRay, const Renderer::Camera& camera, Model::PickResult& pickResult);
        public:
            BBox3 bounds() const;
        public:
        
            // getting highlighted handles
            std::vector<Polygon3f> polygonsHighlightedByDrag() const;

            bool hasDragPolygon() const;
            Polygon3f dragPolygon() const;
            
            /**
             * If inside a drag, returns the bbox at the start of the drag.
             * Otherwise, returns the current bounds(). for rendering sheared bbox.
             */
            BBox3 bboxAtDragStart() const;

            void startShearWithHit(const Model::Hit& hit);
            void commitShear();
            void cancelShear();


            void dragShear(const Vec3& delta);

            const Model::Hit& dragStartHit() const;

            Mat4x4 bboxShearMatrix() const;
            Polygon3f shearHandle() const;

            void updatePickedSide(const Model::PickResult &pickResult);

        private:
            void bindObservers();
            void unbindObservers();
            void nodesDidChange(const Model::NodeList& nodes);
            void selectionDidChange(const Selection& selection);
        };
    }
}

#endif /* defined(TrenchBroom_ShearObjectsTool) */
