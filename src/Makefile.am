# these are the headers for your project
noinst_HEADERS = BallJoint.hh BaseGeom.hh Body.hh BoxGeom.hh CanonVCC4.hh \
	Color.hh ContactParams.hh CylinderGeom.hh DomeGeom.hh Error.hh FrustrumGeom.hh \
	GLContext.hh GarminGPS.hh Geom.hh Global.hh GroundPlane.hh Hinge2Joint.hh \
	HingeJoint.hh IRCEConfiguration.h Image.hh Joint.hh JointGroup.hh LightGeom.hh \
	LightSource.hh Model.hh ModelFactory.hh MonoCam.hh ObserverCam.hh Pioneer2AT.hh \
	Pioneer2DX.hh Pioneer2Gripper.hh PlaneGeom.hh PointSetGeom.hh RayGeom.hh \
	RayProximity.hh RenderOptions.hh RoadGeom.hh Sensor.hh SickLMS200.hh SimpleSolid.hh \
	SkyDome.hh SliderJoint.hh SonyVID30.hh SphereGeom.hh TerrainGeom.hh \
	UniversalJoint.hh Vector.hh WheelGeom.hh World.hh WorldFile.hh dCylinder.h replace.h \
	worldbuilder.h wxIRCEConfigurationDialog.h

#########################################################################
# APPLICATION SECTION
#########################################################################
# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = worldbuilder

# the application source, library search path, and link libraries
worldbuilder_SOURCES = AppConfiguration.cpp AppControlProgram.cpp \
	AppIRCEProject.cpp AppVirtualWorld.cpp ApplicationException.cpp BallJoint.cc BaseGeom.cc \
	Body.cc BoolField.cpp BoxGeom.cc CanonVCC4.cc ConfigurationFile.cpp \
	ContactParams.cc Conversion.cpp CylinderGeom.cc DomeGeom.cc DoubleField.cpp \
	DoubleTuple.cpp DoubleTupleField.cpp Error.cc Exception.cpp ExceptionHandler.cpp \
	Field.cpp FieldDiccionary.cpp FileName.cpp FrustrumGeom.cc GLContext.cc \
	GarminGPS.cc Geom.cc Geometry.cpp GroundPlane.cc GzColorField.cpp GzVectorField.cpp \
	Hinge2Joint.cc HingeJoint.cc IRCEConfiguration.cpp IRCEProject.cpp Image.cc \
	IntegerField.cpp IntegerTuple.cpp IntegerTupleField.cpp Interval.cpp Joint.cc \
	JointGroup.cc LightGeom.cc LightSource.cc Model.cc ModelFactory.cc ModelUtility.cpp \
	MonoCam.cc ObserverCam.cc Pioneer2AT.cc Pioneer2DX.cc Pioneer2Gripper.cc \
	PlaneGeom.cc PlayerConfiguration.cpp PlayerControl.cpp PointSetGeom.cc RayGeom.cc \
	RayProximity.cc RenderOptionsSingleton.cpp RoadGeom.cc Sensor.cc SickLMS200.cc \
	SimpleSolid.cc SimulationConfiguration.cpp SkyDome.cc SliderJoint.cc SonyVID30.cc \
	SphereGeom.cc StringField.cpp SystemException.cpp TerrainGeom.cc UniversalJoint.cc \
	Vector.cc VirtualWorld.cpp WheelGeom.cc World.cc WorldFile.cc XMLCommand.cpp \
	dCylinder.cc dirname.c tinystr.cpp tinyxml.cpp tinyxmlerror.cpp tinyxmlparser.cpp \
	worldbuilder.cpp wx3DChildModelDialog.cpp wx3DModelDialog.cpp wx3DModelPanel.cpp \
	wxClientControlPanel.cpp wxConfigurationPanel.cpp wxIRCEConfigurationDialog.cpp \
	wxIRCEMainWindow.cpp wxIRCEMenuBar.cpp wxModelGLCanvas.cpp wxModelSelectedEvent.cpp \
	wxPlayerConfigurationDialog.cpp wxSimulationConfigurationDialog.cpp
worldbuilder_LDFLAGS = $(WX_LIBS) 

