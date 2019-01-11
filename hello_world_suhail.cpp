/*
Suhail Ansari
Hello World to OpenSceneGraph i.e: OSG
Version: 1.0
Date : 10/12/2018
Release date: 17/12/2018

*/

#include <osg/Group>
#include <osg/Geometry>
#include <osg/Node>
#include <osg/Vec3>
#include <osg/ref_ptr>
#include <osg/Geode>
#include <osg/Notify>
#include <osg/Texture2D>
#include <osg/Transform>
#include <osg/MatrixTransform>
#include <osg/Matrix>
#include <osg/NodeCallback>
#include <osg/Camera>
#include <osg/BlendFunc>
#include <osg/PolygonMode>
#include <osg/ShapeDrawable>


#include <osgGA/TrackballManipulator>
#include <osgGA/GUIEventHandler>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgDB/Registry>

#include <osgText/Font>
#include <osgText/Text>

#include <osgUtil/LineSegmentIntersector>

#include <iostream>
#include <math.h>
#include <dos.h>

using namespace std;
using std::endl;





int ConstantVariation = 5;



class LinearMotionArrow; // forward declaration
class EH;
double x = 0;
double factor = 0.1;
enum keys { FORWARD, BACKWARD, LEFT, RIGHT, STOP, ZOOM_IN, ZOOM_OUT } DIRECTION; // enum in C++
enum mouse { CHANGE, ORIGINAL } COLOR; 
// Now create a class that perform various event handling operations.


osg::ref_ptr<osg::Geometry> createCircle(const float radius, const osg::Vec3 &center);
osg::ref_ptr<osg::Group> createSceneGraph();

osg::Quat rotation;
osg::Matrixd md;
osg::Matrixd originalmd;
float angle = 0;
osgViewer::Viewer viewer;
osg::Vec3d eye = osg::Vec3d(0, 0, 250);
osg::Vec3d up = osg::Vec3d(0, 0, 250);
osg::Vec3d centre = osg::Vec3d(0, 0, -1);

osg::Vec3d original_eye = osg::Vec3d(0, 0, 250);
osg::Vec3d original_up = osg::Vec3d(0, 0, 250);
osg::Vec3d original_centre = osg::Vec3d(0, 0, -1);
class ChangeColor : public osg::NodeCallback{
public:
	ChangeColor() : _color(0.0){}
	virtual void operator()(osg::Node* node,osg::NodeVisitor *nv){
		osg::Geometry* geom = dynamic_cast<osg::Geometry*>(node);
		osg::ref_ptr<osg::Vec4Array> color = new osg::Vec4Array();
		geom->setColorArray(color.get());
		geom->setColorBinding(osg::Geometry::BIND_OVERALL);
		if (COLOR == ORIGINAL){
			color->push_back(osg::Vec4(1, 0, 0, 1));
		}
		else if (COLOR == CHANGE){
			color->push_back(osg::Vec4(0, 1, 0 , 1));
		}
		else{
			color->push_back(osg::Vec4(1, 1, 1, 1));
		}
		traverse(geom, nv);
	}
protected:
	float _color;
};
osg::ref_ptr<osg::Vec4Array> ring_color = new osg::Vec4Array();
osg::ref_ptr<osg::Geometry> createCircle(const float radius, const osg::Vec3& center)
{
	osg::ref_ptr<osg::Geometry> ring = new osg::Geometry;
	osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array;
	//osg::ref_ptr<osg::Vec4Array> ring_color = new osg::Vec4Array;
	osg::ref_ptr<osg::Vec3Array> n = new osg::Vec3Array;
	for (int i = 0; i < 4 * 5800; i++)
	{

		float angle = float(i);

		v->push_back(osg::Vec3(center.x() + radius*cosf(angle), center.y(), center.z() + radius*sinf(angle)));
		v->push_back(osg::Vec3(center.x() + radius*cosf(angle), center.y(), center.z() + radius*sinf(angle) + 0.1));
		v->push_back(osg::Vec3(center.x() + radius*cosf(angle) + 0.1, center.y(), center.z() + radius*sinf(angle) + 0.1));
		v->push_back(osg::Vec3(center.x() + radius*cosf(angle) + 0.1, center.y(), center.z() + radius*sinf(angle)));

	}
	ring->setVertexArray(v.get());
	ring->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLE_STRIP, 0, 4 * 5800));
	ring_color->push_back(osg::Vec4(1.f, 1.f, 0.f, 1.f));
	ring->setColorArray(ring_color.get());
	ring->setColorBinding(osg::Geometry::BIND_OVERALL);
	return ring;
}


class LinearMotionArrow : public osg::NodeCallback{
public:
	LinearMotionArrow() : xAxis(0.0){}
	virtual void operator()(osg::Node *node, osg::NodeVisitor *nv){
		
		osg::Geometry* geom = dynamic_cast<osg::Geometry*> (node);
		osg::ref_ptr<osg::Vec2Array> tc = new osg::Vec2Array();
		geom->setTexCoordArray(0, tc.get());
		tc->push_back(osg::Vec2(-xAxis, 2));
		tc->push_back(osg::Vec2(5 - xAxis, 2));
		tc->push_back(osg::Vec2(5 - xAxis, 3));
		tc->push_back(osg::Vec2(-xAxis, 3));
		if (DIRECTION == FORWARD){
			xAxis += 0.005;
		}
		else if (DIRECTION == BACKWARD){
			xAxis -= 0.005;
		}
		else{
			xAxis += 0.005; // default direction
		}

		/*x += 0.005;*/
		traverse(geom, nv);
	}

protected:
	float xAxis;
};

class SaveGeometry{
public:
	SaveGeometry() {}
	SaveGeometry(osg::ref_ptr<osg::Geometry> geom) : _geom(geom){}
	osg::ref_ptr<osg::Geometry> getGeometry() { return _geom.get(); }
protected:
	osg::ref_ptr<osg::Geometry> _geom;
};
class EH : public osgGA::GUIEventHandler
{
public:
	EH(){}
	EH(SaveGeometry* ring) : _ring(ring->getGeometry()){}
	osg::Node* getOrCreateSelectionBox();
	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
	virtual void pick(osgViewer::View* view, const osgGA::GUIEventAdapter &ea);
	virtual void changeColor(const string name);
protected:
	osg::ref_ptr<osg::Geometry> _ring;
	osg::ref_ptr<osg::MatrixTransform> _selectionBox;
};




int numSegs = 300;
osg::ref_ptr<osg::Vec3Array> getCoordinates(float radius, float thickness)
{
	osg::ref_ptr<osg::Vec3Array> coords = new osg::Vec3Array;

	float startAngle = 0;
	float endAngle = 360;
	float angle = 0;

	// here we are calculating the value that we will increment each time
	float increment = (endAngle - startAngle) / (float)numSegs;
	for (int i = 0; i < numSegs; i++) {
		coords->push_back(osg::Vec3(radius * cosf(osg::RadiansToDegrees(angle)), 0.0f, radius * sinf(osg::RadiansToDegrees(angle))));
		coords->push_back(osg::Vec3(radius * cosf(osg::RadiansToDegrees(angle)) + thickness, 0.0f, radius * sinf(osg::RadiansToDegrees(angle))));
		coords->push_back(osg::Vec3(radius * cosf(osg::RadiansToDegrees(angle)) + thickness, 0.0f, radius * sinf(osg::RadiansToDegrees(angle)) + thickness));
		coords->push_back(osg::Vec3(radius * cosf(osg::RadiansToDegrees(angle)), 0.0f, radius * sinf(osg::RadiansToDegrees(angle)) + thickness));
		angle += increment;
	}
	return coords.release();
}

osg::ref_ptr<osg::Geometry> drawRings(float radius, float thickness)
{
	//Create object to store geometry in
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;

	//Get the coordinate mapping to draw the quad strip accordingly
	osg::ref_ptr<osg::Vec3Array> coords = getCoordinates(radius, thickness); //pass radius which currently is hard coded

	//set it as vertex array
	geom->setVertexArray(coords.get());

	// Create an array for the single normal.
	osg::ref_ptr<osg::Vec3Array> n = new osg::Vec3Array;
	geom->setNormalArray(n.get());
	geom->setNormalBinding(osg::Geometry::BIND_OVERALL);
	n->push_back(osg::Vec3(0.f, -1.f, 0.f));

	// Draw a quad strip with the same number of segments as taken in the getCoordinates(int) method
	geom->addPrimitiveSet(
		new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP, 0, numSegs * 4));

	return geom.get();
}

void EH::pick(osgViewer::View* view, const osgGA::GUIEventAdapter &ea){
	osgUtil::LineSegmentIntersector::Intersections intersections;
	if (view->computeIntersections(ea, intersections)){
		for (osgUtil::LineSegmentIntersector::Intersections::iterator hitr = intersections.begin();
			hitr != intersections.end(); ++hitr){
			std::ostringstream os;
			if (!hitr->nodePath.empty() && !(hitr->nodePath.back()->getName().empty())){
				if (hitr->nodePath.back()->getName() == "ring"){
					osg::ref_ptr<osg::Vec4Array> color1 = new osg::Vec4Array();
					color1->push_back(osg::Vec4(0, 1, 0, 1));
					osg::ref_ptr<osg::Vec4Array> color2 = new osg::Vec4Array();
					color2->push_back(osg::Vec4(1, 0, 0, 1));
					switch (ea.getEventType())
					{
					case osgGA::GUIEventAdapter::PUSH:
						switch (ea.getButton())
						{
						case osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON:
							_ring->setColorArray(color1.get());
							_ring->setColorBinding(osg::Geometry::BIND_OVERALL);
							break;
						}
					case osgGA::GUIEventAdapter::DOUBLECLICK:
						_ring->setColorArray(color2.get());
						_ring->setColorBinding(osg::Geometry::BIND_OVERALL);
					}
				}
			}
		}
	}

}

osg::Node* EH::getOrCreateSelectionBox(){
	if (!_selectionBox){
		osg::ref_ptr<osg::Geode> geode = new osg::Geode();
		geode->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(), 1)));
		_selectionBox = new osg::MatrixTransform();
		_selectionBox->setNodeMask(0x1);
		_selectionBox->addChild(geode.get());
		osg::StateSet* ss = _selectionBox->getOrCreateStateSet();
		ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
		ss->setAttributeAndModes(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK,
			osg::PolygonMode::LINE));
	}
	return _selectionBox.get();
}



#include <thread>
#include <chrono>

void EH::changeColor(const string name){
	osg::ref_ptr<osg::Vec4Array> color1 = new osg::Vec4Array();
	color1->push_back(osg::Vec4(0, 1, 0, 1));
	_ring->setColorArray(color1.get());
	_ring->setColorBinding(osg::Geometry::BIND_OVERALL);
}
osg::ref_ptr<osg::Vec4Array> color1;
bool EH::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	osgViewer::Viewer* viewer1 = dynamic_cast<osgViewer::Viewer*>(&aa);
	osg::ref_ptr<osg::Camera> camera = viewer1->getCamera();
	//osg::Vec3d eye, centre, up;
	osg::ref_ptr<osg::Vec4Array> color = new osg::Vec4Array();
	color->push_back(osg::Vec4(1, 0, 0, 1));
	if (ea.getEventType() == osgGA::GUIEventAdapter::PUSH || ea.getEventType() == osgGA::GUIEventAdapter::DOUBLECLICK || ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON) 
	{
		if (viewer1)
		{
			osg::Vec3d start_point, end_point;

			osg::Matrix inVPW;
			osg::Matrix VPW = camera->getViewMatrix()*camera->getProjectionMatrix()*camera->getViewport()->computeWindowMatrix();
			inVPW.invert(VPW);

			start_point = osg::Vec3f(ea.getX(), ea.getY(), 0.f) * inVPW;
			end_point = osg::Vec3f(ea.getX(), ea.getY(), 1.f) * inVPW;

			/*start_point = osg::Vec3d(0, 0, 0);
			end_point = osg::Vec3d(0, 1000,0);*/
			//osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector(osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());
			osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector(start_point,end_point);

			osgUtil::IntersectionVisitor iv(intersector.get());
			iv.setTraversalMask(~0x1);
			viewer1->getSceneData()->accept(iv);
			if (intersector->containsIntersections()){
				osgUtil::LineSegmentIntersector::Intersection& result = intersector->getFirstIntersection();
				osg::NodePath nodePath = result.nodePath;
				osg::Node* node = NULL;
				if (nodePath.size() > 0)
				node = nodePath[nodePath.size() - 1];
				if (node != NULL)
				{
					using namespace std::this_thread;
					using namespace std::chrono;
					using std::chrono::system_clock;


					std::cout << "inside color\n";
				
					std::string name = node->getName();
					osg::Geode *geo = node->asGeode();
					//	osg::Geometry* geom =
					osg::ref_ptr<osg::Drawable> drw = geo->getDrawable(0);
					osg::ref_ptr<osg::Geometry> ring = drw->asGeometry();
					osg::ref_ptr<osg::Vec4Array> c = dynamic_cast<osg::Vec4Array*>(ring->getColorArray());
					osg::Vec4 ff = {1,1,1,1} ;
					c[0][0] = ff;
					if (name == "ring")
					{
						if (ea.getEventType() == osgGA::GUIEventAdapter::DOUBLECLICK){
							cout << "red color\n";
							ring_color->push_back(osg::Vec4(1.f, 0.f, 0.f, 1.f));
							ring->setColorArray(ring_color.get(), osg::Array::BIND_OVERALL);
							sleep_for(nanoseconds(10000));
							sleep_until(system_clock::now()+seconds(1));
						}
						else if (ea.getEventType() == osgGA::GUIEventAdapter::PUSH){
							cout << "green color\n";
							ring_color->push_back(osg::Vec4(0.f, 1.f, 0.f, 1.f));
							ring->setColorArray(ring_color.get(), osg::Array::BIND_OVERALL);
							sleep_for(nanoseconds(10000));
							sleep_until(system_clock::now() + seconds(1));
							if (ea.getEventType() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON){
								cout << "blue color\n";
								ring_color->push_back(osg::Vec4(0.f, 0.f, 1.f, 1.f));
								ring->setColorArray(ring_color.get(), osg::Array::BIND_OVERALL);
								sleep_for(nanoseconds(10000));
								sleep_until(system_clock::now() + seconds(1));
							}
							
						}
						else{
							cout << "white color\n";
							ring_color->push_back(osg::Vec4(0, 1.f, 0, 1.f));
							ring->setColorArray(ring_color.get(), osg::Array::BIND_OVERALL);
							sleep_for(nanoseconds(10000));
							sleep_until(system_clock::now() + seconds(1));
						}
					}
				}
			}
		}
		
	}
	if (camera.valid()){
		camera->getViewMatrixAsLookAt(eye, centre, up);
	}
	switch (ea.getEventType())
	{
	case(osgGA::GUIEventAdapter::FRAME) :
		cout << "Inside switch" << std::endl;
		break;

	case osgGA::GUIEventAdapter::SCROLL:
	{

		switch (ea.getScrollingMotion())
		{
		case osgGA::GUIEventAdapter::SCROLL_DOWN:
			if (eye.y() > -200)
			{
				eye += osg::Vec3d(0, -2, 0);
				up = osg::Vec3d(0, 0, 1);
				centre = osg::Vec3d(0, 0, 0);
				camera->setViewMatrixAsLookAt(eye, centre, up);
			}
			return false;
		case osgGA::GUIEventAdapter::SCROLL_UP:
			if (eye.y() < -45)
			{
				eye += osg::Vec3d(0, 2, 0);
				up = osg::Vec3d(0, 0, 1);
				centre = osg::Vec3d(0, 0, 0);
				camera->setViewMatrixAsLookAt(eye, centre, up);
			}
			return false;
		}
		return false;
	}
	case (osgGA::GUIEventAdapter::KEYDOWN) :
		
		if (ea.getKey() == osgGA::GUIEventAdapter::KEY_8)
		{
			osg::Matrixd &viewMatrix = viewer.getCamera()->getViewMatrix();
			osg::Matrix mat;
			osg::Matrix rot = osg::Matrix::rotate(osg::PI/18, osg::Vec3d(0, 0, 1));
			mat = osg::Matrix::inverse(viewMatrix)*rot;
			viewMatrix = mat;
		}
		if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Space){
			md = originalmd;
		}
		switch (ea.getKey())
		{
		case osgGA::GUIEventAdapter:: KEY_Left :
			DIRECTION = BACKWARD;
			return true;
		case osgGA::GUIEventAdapter::KEY_Right:
			DIRECTION = FORWARD;
			return true;
		case osgGA::GUIEventAdapter::KEY_L:
			if (eye.x() > -1){
				eye -= osg::Vec3d(0.001, 0, 0);
				camera->setViewMatrixAsLookAt(eye, centre, up);
			}
			return false;
		case osgGA::GUIEventAdapter::KEY_R:
			if (eye.x() < 1){
				eye += osg::Vec3d(0.001, 0, 0);
				camera->setViewMatrixAsLookAt(eye, centre, up);
			}
			return false;
		case osgGA::GUIEventAdapter::KEY_Up:
			if (eye.z() > -1){
				eye += osg::Vec3d(0, 0, 0.001);
				camera->setViewMatrixAsLookAt(eye, centre, up);
			}
			return false;
		case osgGA::GUIEventAdapter::KEY_Down:
			if (eye.z() < 1){
				eye -= osg::Vec3d(0, 0, 0.001);
				camera->setViewMatrixAsLookAt(eye, centre, up);
			}
			return false;
		case osgGA::GUIEventAdapter::KEY_Plus:
			DIRECTION = ZOOM_IN;
			return true;
		case osgGA::GUIEventAdapter::KEY_Minus:
			DIRECTION = ZOOM_OUT;
			return true;
		case osgGA::GUIEventAdapter::KEY_W:
			std::cout << "w key pressed " << std::endl;
			return false;
		case osgGA::GUIEventAdapter::KEYUP:
			switch (ea.getKey())
			{
			case osgGA::GUIEventAdapter::KEY_A:
			case osgGA::GUIEventAdapter::KEY_D:
			case osgGA::GUIEventAdapter::KEY_Plus:
			case osgGA::GUIEventAdapter::KEY_Minus:
				DIRECTION = STOP;
				return true;
			}
		}
	}
	return false;
}


osg::ref_ptr<osg::Group> createSceneGraph(){

	osg::ref_ptr<osg::Geode> geode5 = new osg::Geode();

	osg::ref_ptr<osg::Geometry> ring = createCircle(2.4, osg::Vec3(0, 0, 0));

	SaveGeometry* savedRing = new SaveGeometry(ring.get());
	
//	ring->setEventCallback(new EH(savedRing));
	osg::ref_ptr<osg::Geode> ring_geode = new osg::Geode();
	ring_geode->addDrawable(ring);
	ring_geode->setName("ring");
	//ring_geode->addEventCallback(new EH());
	//ring_geode->addEventCallback(new EH(savedRing));

	osg::ref_ptr<osg::MatrixTransform> ring1 = new osg::MatrixTransform();
	ring1->setMatrix(osg::Matrix::translate(-2.4, 0, 0));
	osg::ref_ptr<osg::MatrixTransform> ring2 = new osg::MatrixTransform();
	ring2->setMatrix(osg::Matrix::translate(7.5, 0, 2.4));
	ring1->addChild(ring_geode);
	ring1->setName("ring1");
	ring2->addChild(ring_geode);
	ring1->setName("ring2");

	osg::ref_ptr<osg::Group> root = new osg::Group();
	root->addChild(ring1.get());
	root->addChild(ring2.get());

	// Make a group node 
	
	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	//.. Create the first geometry make a quad and apply texture property to it.
	osg::ref_ptr<osg::Geometry> geom1 = new osg::Geometry();
	osg::ref_ptr<osg::Vec3Array> v1 = new osg::Vec3Array();
	// add the vertex array to the geom1
	geom1->setVertexArray(v1.get());
	v1->push_back(osg::Vec3(-20, 0, 0));
	v1->push_back(osg::Vec3(-5, 0, 0));
	v1->push_back(osg::Vec3(-5, 0, 5));
	v1->push_back(osg::Vec3(-20, 0, 5));

	// define a color array
	osg::ref_ptr<osg::Vec4Array> c1 = new osg::Vec4Array();
	// set the color array to geom1
	geom1->setColorArray(c1.get());
	// set color Bindings
	geom1->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	c1->push_back(osg::Vec4(1, 1, 1, 1));
	c1->push_back(osg::Vec4(1, 1, 1, 1));
	c1->push_back(osg::Vec4(1, 1, 1, 1));
	c1->push_back(osg::Vec4(1, 1, 1, 1));
	// declare a normal vector and set the normal vector in the outward direction
	osg::ref_ptr<osg::Vec3Array> n1 = new osg::Vec3Array();
	// set the normal binding
	geom1->setNormalArray(n1.get());
	geom1->setNormalBinding(osg::Geometry::BIND_OVERALL);
	n1->push_back(osg::Vec3(0, -1, 0));

	// Draw a four vertex quad from the stored data.
	geom1->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));

	// add the geometry to a geode and add geode to the group;



	// this is for second quads

	osg::ref_ptr<osg::Geometry> geom2 = new osg::Geometry();
	osg::ref_ptr<osg::Vec3Array> v2 = new osg::Vec3Array();
	// add the vertex array to the geom2
	geom2->setVertexArray(v2.get());
	v2->push_back(osg::Vec3(10, 0, 0));
	v2->push_back(osg::Vec3(20, 0, 0));
	v2->push_back(osg::Vec3(20, 0, 5));
	v2->push_back(osg::Vec3(10, 0, 5));

	// define a color array
	osg::ref_ptr<osg::Vec4Array> c2 = new osg::Vec4Array();
	// set the color array to geom2
	geom2->setColorArray(c2.get());
	// set color Bindings
	geom2->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	c2->push_back(osg::Vec4(1, 1, 0, 0.3));
	c2->push_back(osg::Vec4(1, 1, 0, 0.3));
	c2->push_back(osg::Vec4(1, 1, 0, 0.3));
	c2->push_back(osg::Vec4(1, 1, 0, 0.3));
	/*c2->push_back(osg::Vec4(1, 1, 1, 1));
	c2->push_back(osg::Vec4(1, 1, 1, 1));
	c2->push_back(osg::Vec4(1, 1, 1, 1));
*/
	// declare a normal vector and set the normal vector in the outward direction
	osg::ref_ptr<osg::Vec3Array> n2 = new osg::Vec3Array();
	// set the normal binding
	geom2->setNormalArray(n2.get());
	geom2->setNormalBinding(osg::Geometry::BIND_OVERALL);
	n2->push_back(osg::Vec3(0, -1, 0));

	// Draw a four vertex quad from the stored data.
	geom2->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));

	// Create a Vec2Array of texture coordinates for texture unit 0
	// and attach it to the geom1 and geom2
	osg::ref_ptr<osg::Vec2Array> tc = new osg::Vec2Array();
	geom1->setTexCoordArray(0, tc.get());
	geom2->setTexCoordArray(0, tc.get());
	tc->push_back(osg::Vec2(0, 0));
	tc->push_back(osg::Vec2(1, 0));
	tc->push_back(osg::Vec2(1, 1));
	tc->push_back(osg::Vec2(0, 1));



	// Load the texture image
	osg::ref_ptr<osg::Image> image1 = osgDB::readImageFile("D:\\Data_Suhail\\HELL.jpg");
	osg::ref_ptr<osg::Image> image2 = osgDB::readImageFile("D:\\Data_Suhail\\RLD.jpg");
	/*osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc();
	blendFunc->setFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/


	// Attach the image in a Texture2D object.

	osg::ref_ptr<osg::Texture2D> tex1 = new osg::Texture2D();
	osg::ref_ptr<osg::Texture2D> tex2 = new osg::Texture2D();


	

	// After creating the OpenGL texture object, release the
	// internal ref_ptr<Image> (delete the Image)
	tex1->setUnRefImageDataAfterApply(true);
	tex2->setUnRefImageDataAfterApply(true);
	//

	// add the geometry to a geode and add geode to the group;
	osg::ref_ptr<osgText::Font> font = osgText::readFontFile("fonts/cour.ttf");
	osg::ref_ptr<osgText::Text> text = new osgText::Text();
	text->setFont(font.get());
	text->setText("W");
	// set the position of text
	text->setPosition(osg::Vec3(0, 0, 0));
	// orientation
	text->setAxisAlignment(osgText::Text::SCREEN);
	// Size of the specified text
	text->setCharacterSize(8);
	// Resolution
	text->setFontResolution(128, 128);
	// color of the text
	text->setColor(osg::Vec4(1, 1, 1, 1));

	// Make the base QUAD

	osg::ref_ptr<osg::Geometry> geom3 = new osg::Geometry();
	osg::ref_ptr<osg::Vec3Array> v3 = new osg::Vec3Array();
	// add the vertex array to the geom1
	geom3->setVertexArray(v3.get());

	v3->push_back(osg::Vec3(-20, 0, -7));
	v3->push_back(osg::Vec3(20, 0, -7));
	v3->push_back(osg::Vec3(20, 0, -2));
	v3->push_back(osg::Vec3(-20, 0, -2));

	// define a color array
	osg::ref_ptr<osg::Vec4Array> c3 = new osg::Vec4Array();
	// set the color array to geom1
	geom3->setColorArray(c3.get());
	// set color Bindings
	geom3->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	c3->push_back(osg::Vec4(1, 1, 1, 1));
	c3->push_back(osg::Vec4(1, 1, 1, 1));
	c3->push_back(osg::Vec4(1, 1, 1, 1));
	c3->push_back(osg::Vec4(1, 1, 1, 1));
	// declare a normal vector and set the normal vector in the outward direction
	osg::ref_ptr<osg::Vec3Array> n3 = new osg::Vec3Array();
	// set the normal binding
	geom3->setNormalArray(n3.get());
	geom3->setNormalBinding(osg::Geometry::BIND_OVERALL);
	n3->push_back(osg::Vec3(0, -1, 0));

	// Draw a four vertex quad from the stored data.
	geom3->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));
	osg::ref_ptr<osg::Vec2Array> tc_base = new osg::Vec2Array();
	geom3->setTexCoordArray(0, tc_base.get());

	tc_base->push_back(osg::Vec2(0, 1));
	tc_base->push_back(osg::Vec2(10, 1));
	tc_base->push_back(osg::Vec2(10, 3));
	tc_base->push_back(osg::Vec2(0, 3));

	osg::ref_ptr<osg::Image> base_image = osgDB::readImageFile("D:\\Data_Suhail\\Arrow.jpg");

	osg::ref_ptr<osg::Texture2D> tex3 = new osg::Texture2D();
	// This code can move the texture over the quad;

	tex3->setDataVariance(osg::Object::DYNAMIC);
	tex3->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
	tex3->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
	//geom3->addEventCallback(new CustEventHandler());
	geom3->setUpdateCallback(new LinearMotionArrow());
	tex3->setImage(base_image.get());
	geom3->getOrCreateStateSet()->setTextureAttributeAndModes(0, tex3.get());
	tex3->setUnRefImageDataAfterApply(true);


	osg::ref_ptr<osg::Geode> geode1 = new osg::Geode();
	osg::ref_ptr<osg::Geode> geode2 = new osg::Geode();
	osg::ref_ptr<osg::Geode> geode3 = new osg::Geode();
	osg::ref_ptr<osg::Geode> geode4 = new osg::Geode();
	geode1->addDrawable(text.get());
	geode2->addDrawable(geom1.get());
	geode3->addDrawable(geom2.get());
	geode4->addDrawable(geom3.get());

	tex1->setImage(image1.get());
	tex2->setImage(image2.get());

	osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc();
	blendFunc->setFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	osg::StateSet* stateSet = geode2->getOrCreateStateSet();
	stateSet->setTextureAttributeAndModes(0, tex1.get());
	stateSet->setAttributeAndModes(blendFunc);
	stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

	
	root->addChild(geode1.get());
	root->addChild(geode2.get());
	root->addChild(geode3.get());
	root->addChild(geode4.get());
	root->addChild(geode5.get());
	//root->addChild(ring_geode.get());
	return root.get();


}




int main(int argc, char* argv[]){

	viewer.addEventHandler(new osgViewer::WindowSizeHandler());
	viewer.setUpViewOnSingleScreen();
	osg::Vec3d eye = osg::Vec3d(0, 0, 250);
	osg::Vec3d up = osg::Vec3d(0, 0, 250);
	osg::Vec3d centre = osg::Vec3d(0, 0, -1);
	
	osg::Camera* camera = viewer.getCamera();
	eye = osg::Vec3d(0, -300, 0);
	up = osg::Vec3d(0, 0, 1);
	centre = osg::Vec3d(0, 0, 0);
	camera->setViewMatrixAsLookAt(eye, centre, up);

	camera->getViewMatrixAsLookAt(eye, centre, up);	
	camera->setViewport(0, 0, 1920, 1080);
	osg::ref_ptr<osg::Group> root = createSceneGraph();

	viewer.setSceneData(root);

	EH* eh = new EH();
	viewer.addEventHandler(eh);
	
	//const osg::BoundingSphere& bs = root->getBound();
	///viewer.getCamera()->setViewMatrixAsLookAt(bs.center() + osg::Vec3(0, -(3 * bs.radius()), 0), bs.center(), osg::Z_AXIS);

	md = viewer.getCamera()->getViewMatrix();
	originalmd = md;
	//viewer.realize();
	
	//viewer->run();
	while (!viewer.done()){
		//viewer.getCamera()->setViewMatrix(md);
		viewer.frame();
	}
}
