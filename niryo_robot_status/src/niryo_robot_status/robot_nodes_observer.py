import rospy
import rosnode

from niryo_robot_msgs.srv import Ping, PingResponse


class RobotNodesObserver(object):

    def __init__(self, robot_status_handler):
        """

        :type robot_status_handler: RobotStatusHandler
        """
        self.__robot_status_handler = robot_status_handler

        self.__log_nodes = rospy.get_param("~node_logs")
        self.__initialization_params = rospy.get_param("~initialisation_params")

        self.__vital_nodes = rospy.get_param("~vital_nodes_common")
        if rospy.has_param('/niryo_robot/simulation_mode') and not rospy.get_param('/niryo_robot/simulation_mode'):
            self.__vital_nodes += rospy.get_param("~vital_nodes_real")

        self.__are_vital_nodes_alive = False
        self.__missing_vital_nodes = []
        self.__not_initialized_nodes = []
        self.__check_nodes_timer = None

        self.__python_wrapper_nodes = set()
        self.__advertise_python_ros_wrapper = rospy.Service('~ping_ros_wrapper', Ping,
                                                            self.__callback_python_ros_wrapper_node)

    def __callback_python_ros_wrapper_node(self, req):
        if req.name not in ['/niryo_robot_user_interface', '']:
            if req.state:
                self.__python_wrapper_nodes.add(req.name)
            elif req.name in self.__python_wrapper_nodes:
                self.__python_wrapper_nodes.remove(req.name)
            self.__robot_status_handler.advertise_new_state()
        return PingResponse()

    @property
    def check_user_node(self):
        return bool(self.__python_wrapper_nodes)

    def check_vital_nodes(self):
        alive_nodes = rosnode.get_node_names()

        self.__python_wrapper_nodes = {pywrapper_node for pywrapper_node in self.__python_wrapper_nodes if
                                       pywrapper_node in alive_nodes}
        missing_nodes = [vital_node for vital_node in self.__vital_nodes if vital_node not in alive_nodes]

        self.__are_vital_nodes_alive = not bool(missing_nodes)
        self.__missing_vital_nodes = missing_nodes

        return self.__are_vital_nodes_alive

    def check_nodes_initialization(self):
        if not self.check_vital_nodes():
            return False

        missing_params = []
        for initialisation_param in self.__initialization_params:
            if not rospy.has_param(initialisation_param) or not rospy.get_param(initialisation_param):
                missing_params.append(initialisation_param)

        self.__not_initialized_nodes = missing_params
        return not self.__not_initialized_nodes

    def start_nodes_check_loop(self):
        if self.__check_nodes_timer is None:
            self.__check_nodes_timer = rospy.Timer(rospy.Duration(1. / rospy.get_param("~check_nodes_frequency")),
                                                   self.__check_vital_nodes_callback)

    def __check_vital_nodes_callback(self, _):
        self.check_vital_nodes()

        if not self.__are_vital_nodes_alive:
            self.__robot_status_handler.advertise_new_state()

    @property
    def are_vital_nodes_alive(self):
        return self.__are_vital_nodes_alive

    @property
    def missing_vital_nodes(self):
        return self.__missing_vital_nodes

    @property
    def not_initialized_nodes(self):
        return self.__not_initialized_nodes