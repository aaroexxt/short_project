/*  task3
This file includes the required code to implement task3: move planar mechanism with haptic device.

Author: Shameek Ganguly shameekg@stanford.edu
Date: 11/26/18
*/

#include <iostream>
#include <string>
#include <thread>
#include <math.h>

#include "Sai2Model.h"
#include "Sai2Graphics.h"
#include "chai3d.h"

#include "timer/LoopTimer.h"

#include <GLFW/glfw3.h> //must be loaded after loading opengl/glew as part of graphicsinterface

using namespace std;
using namespace chai3d;

const string world_fname = "resources/task3/world.urdf";
const string robot_fname = "resources/task3/RR.urdf";
const string robot_name = "RRBot";
const string camera_name = "camera_top";
const string ee_link_name = "link1";

/*  ----- TASK3 Variables ----- */
bool fHapticDeviceEnabled = false;
Eigen::Vector3d haptic_device_velocity = Eigen::Vector3d::Zero();

/* ----------------------------- */

// simulation loop
bool fSimulationRunning = false;
void simulation(Sai2Model::Sai2Model* robot);
void haptic(cGenericHapticDevicePtr device);

// initialize window manager
GLFWwindow* glfwInitialize();

// callback to print glfw errors
void glfwError(int error, const char* description);

// callback when a key is pressed
void keySelect(GLFWwindow* window, int key, int scancode, int action, int mods);

int main (int argc, char** argv) {
	cout << "Loading URDF world model file: " << world_fname << endl;

	// load graphics scene
	auto graphics = new Sai2Graphics::Sai2Graphics(world_fname, false);

	// load robots
	auto robot = new Sai2Model::Sai2Model(robot_fname, false);

	// set initial condition
	robot->_q << 0.0/180.0*M_PI,
				90.0/180.0*M_PI;
	robot->updateModel();
	// Eigen::Affine3d ee_trans;
	// robot->transform(ee_trans, ee_link_name);
	// cout << ee_trans.translation().transpose() << endl;
	// cout << ee_trans.rotation() << endl;

	// initialize GLFW window
	GLFWwindow* window = glfwInitialize();

    // set callbacks
	glfwSetKeyCallback(window, keySelect);

	// start the simulation
	thread sim_thread(simulation, robot);

	// initialize the haptic device
	auto handler = new cHapticDeviceHandler();
	cGenericHapticDevicePtr hapticDevice;
	handler->getDevice(hapticDevice, 0);
	if (NULL == hapticDevice) {
		cerr << "No haptic device found. " << endl;
		fHapticDeviceEnabled = false;
	} else {
		cout << "Haptic device initialized and ready. " << endl;
		hapticDevice->open();
		hapticDevice->calibrate();
		fHapticDeviceEnabled = true;
		// if the device has a gripper, enable the gripper to simulate a user switch
	    hapticDevice->setEnableGripperUserSwitch(true);
	}

	thread haptics_thread(haptic, hapticDevice);

    // while window is open:
    while (!glfwWindowShouldClose(window)) {
		// update kinematic models
		// robot->updateModel();

		// update graphics. this automatically waits for the correct amount of time
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		graphics->updateGraphics(robot_name, robot);
		graphics->render(camera_name, width, height);
		glfwSwapBuffers(window);
		glFinish();

	    // poll for events
	    glfwPollEvents();
	}

	// stop simulation
	fSimulationRunning = false;
	sim_thread.join();
	haptics_thread.join();

    // destroy context
    glfwDestroyWindow(window);

    // terminate
    glfwTerminate();

	return 0;
}

//------------------------------------------------------------------------------
void simulation(Sai2Model::Sai2Model* robot) {
	fSimulationRunning = true;

	// create a timer
	LoopTimer timer;
	timer.initializeTimer();
	timer.setLoopFrequency(500); //500Hz timer
	double last_time = timer.elapsedTime(); //secs

	double vhx = 0.0;
	double vhy = 0.0;
	double vhz = 0.0;
	double q0 = 0.0;
	double q1 = 0.0;
	double dq0 = 0.0;
	double dq1 = 0.0;

	double scale = 1.0;

	bool fTimerDidSleep = true;
	while (fSimulationRunning) {
		fTimerDidSleep = timer.waitForNextLoop();

		// integrate joint velocity to joint positions
		double curr_time = timer.elapsedTime();
		double loop_dt = curr_time - last_time; 
		robot->_q += robot->_dq*loop_dt;

		// if (!fTimerDidSleep) {
		// 	cout << "Warning: timer underflow! dt: " << loop_dt << "\n";
		// }

		// update kinematic models
		robot->updateModel();
		
		// get q0, q1
		q0 = robot->_q[0]; // in radians
		q1 = robot->_q[1]; // in radians

		// get haptic device velocity
		vhx = haptic_device_velocity[0];
		vhy = haptic_device_velocity[1];
		vhz = haptic_device_velocity[2];
		
		// ------------------------------------
		// FILL ME IN: set new joint velocities given vxe, vye, q0 and q1
		// NOTE: These should be entered in radians/second. Not in degrees/second.
		dq0 = 0.0;
		dq1 = 0.0;

		// ------------------------------------

		robot->_dq << dq0, dq1;
		
		// ------------------------------------

		// update last time
		last_time = curr_time;
	}
}

//------------------------------------------------------------------------------
void haptic(cGenericHapticDevicePtr device) {
	cVector3d device_vel;
	while (fHapticDeviceEnabled && fSimulationRunning) {
		device->getLinearVelocity(device_vel);
		haptic_device_velocity = device_vel.eigen();
		// cout << haptic_device_velocity << endl;
	}
}

//------------------------------------------------------------------------------
GLFWwindow* glfwInitialize() {
		/*------- Set up visualization -------*/
    // set up error callback
    glfwSetErrorCallback(glfwError);

    // initialize GLFW
    glfwInit();

    // retrieve resolution of computer display and position window accordingly
    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primary);

    // information about computer screen and GLUT display window
	int screenW = mode->width;
    int screenH = mode->height;
    int windowW = 0.8 * screenH;
    int windowH = 0.5 * screenH;
    int windowPosY = (screenH - windowH) / 2;
    int windowPosX = windowPosY;

    // create window and make it current
    glfwWindowHint(GLFW_VISIBLE, 0);
    GLFWwindow* window = glfwCreateWindow(windowW, windowH, "SAI2.0 - Task 3", NULL, NULL);
	glfwSetWindowPos(window, windowPosX, windowPosY);
	glfwShowWindow(window);
    glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	return window;
}

//------------------------------------------------------------------------------

void glfwError(int error, const char* description) {
	cerr << "GLFW Error: " << description << endl;
	exit(1);
}

//------------------------------------------------------------------------------

void keySelect(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // option ESC: exit
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        // exit application
         glfwSetWindowShouldClose(window, 1);
    }
}
