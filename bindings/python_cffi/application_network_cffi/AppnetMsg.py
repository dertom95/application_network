################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Read the zproject/README.md for information about making permanent changes. #
################################################################################
from . import utils
from . import destructors
libapplication_network_destructors = destructors.lib

class AppnetMsg(object):
    """

    """

    @staticmethod
    def create_trigger_action(action_name, args):
        """
        Create trigger action msg (as zmsg) with string argument
        """
        return utils.lib.appnet_msg_create_trigger_action(utils.to_bytes(action_name), utils.to_bytes(args))

    @staticmethod
    def create_trigger_action_data(action_name, data, size):
        """
        Create trigger action msg (as zmsg) with data-buffer
        """
        return utils.lib.appnet_msg_create_trigger_action_data(utils.to_bytes(action_name), data._p, size)

    @staticmethod
    def create_generic_string_list_message(msg_type, amount, string_data, *string_data_args):
        """
        Create a generic zmsg with multiple strings packed as individual zframes.
        """
        return utils.lib.appnet_msg_create_generic_string_list_message(utils.to_bytes(msg_type), amount, utils.to_bytes(string_data), *string_data_args)

    @staticmethod
    def test(verbose):
        """
        Self test of this class.
        """
        utils.lib.appnet_msg_test(verbose)

################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Read the zproject/README.md for information about making permanent changes. #
################################################################################
