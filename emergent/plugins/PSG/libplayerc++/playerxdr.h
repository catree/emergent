/** @ingroup libplayerxdr
 @{ */

#ifndef _PLAYERXDR_PACK_H_
#define _PLAYERXDR_PACK_H_

#include <rpc/types.h>
#include <rpc/xdr.h>

#include "player.h"

#include "functiontable.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef XDR_ENCODE
  #define XDR_ENCODE 0
#endif
#ifndef XDR_DECODE
  #define XDR_DECODE 1
#endif
#define PLAYERXDR_ENCODE XDR_ENCODE
#define PLAYERXDR_DECODE XDR_DECODE

#define PLAYERXDR_MSGHDR_SIZE 40

#define PLAYERXDR_MAX_MESSAGE_SIZE (4*PLAYER_MAX_MESSAGE_SIZE)

int xdr_player_devaddr_t(XDR* xdrs, player_devaddr_t* msg);
int player_devaddr_pack(void* buf, size_t buflen, player_devaddr_t* msg, int op);
int xdr_player_msghdr_t(XDR* xdrs, player_msghdr_t* msg);
int player_msghdr_pack(void* buf, size_t buflen, player_msghdr_t* msg, int op);
int xdr_player_point_2d_t(XDR* xdrs, player_point_2d_t* msg);
int player_point_2d_pack(void* buf, size_t buflen, player_point_2d_t* msg, int op);
int xdr_player_point_3d_t(XDR* xdrs, player_point_3d_t* msg);
int player_point_3d_pack(void* buf, size_t buflen, player_point_3d_t* msg, int op);
int xdr_player_pose_t(XDR* xdrs, player_pose_t* msg);
int player_pose_pack(void* buf, size_t buflen, player_pose_t* msg, int op);
int xdr_player_pose3d_t(XDR* xdrs, player_pose3d_t* msg);
int player_pose3d_pack(void* buf, size_t buflen, player_pose3d_t* msg, int op);
int xdr_player_bbox_t(XDR* xdrs, player_bbox_t* msg);
int player_bbox_pack(void* buf, size_t buflen, player_bbox_t* msg, int op);
int xdr_player_bbox3d_t(XDR* xdrs, player_bbox3d_t* msg);
int player_bbox3d_pack(void* buf, size_t buflen, player_bbox3d_t* msg, int op);
int xdr_player_segment_t(XDR* xdrs, player_segment_t* msg);
int player_segment_pack(void* buf, size_t buflen, player_segment_t* msg, int op);
int xdr_player_color_t(XDR* xdrs, player_color_t* msg);
int player_color_pack(void* buf, size_t buflen, player_color_t* msg, int op);
int xdr_player_bool_t(XDR* xdrs, player_bool_t* msg);
int player_bool_pack(void* buf, size_t buflen, player_bool_t* msg, int op);
int xdr_player_actarray_actuator_t(XDR* xdrs, player_actarray_actuator_t* msg);
int player_actarray_actuator_pack(void* buf, size_t buflen, player_actarray_actuator_t* msg, int op);
int xdr_player_actarray_data_t(XDR* xdrs, player_actarray_data_t* msg);
int player_actarray_data_pack(void* buf, size_t buflen, player_actarray_data_t* msg, int op);
int xdr_player_actarray_actuatorgeom_t(XDR* xdrs, player_actarray_actuatorgeom_t* msg);
int player_actarray_actuatorgeom_pack(void* buf, size_t buflen, player_actarray_actuatorgeom_t* msg, int op);
int xdr_player_actarray_geom_t(XDR* xdrs, player_actarray_geom_t* msg);
int player_actarray_geom_pack(void* buf, size_t buflen, player_actarray_geom_t* msg, int op);
int xdr_player_actarray_position_cmd_t(XDR* xdrs, player_actarray_position_cmd_t* msg);
int player_actarray_position_cmd_pack(void* buf, size_t buflen, player_actarray_position_cmd_t* msg, int op);
int xdr_player_actarray_speed_cmd_t(XDR* xdrs, player_actarray_speed_cmd_t* msg);
int player_actarray_speed_cmd_pack(void* buf, size_t buflen, player_actarray_speed_cmd_t* msg, int op);
int xdr_player_actarray_home_cmd_t(XDR* xdrs, player_actarray_home_cmd_t* msg);
int player_actarray_home_cmd_pack(void* buf, size_t buflen, player_actarray_home_cmd_t* msg, int op);
int xdr_player_actarray_power_config_t(XDR* xdrs, player_actarray_power_config_t* msg);
int player_actarray_power_config_pack(void* buf, size_t buflen, player_actarray_power_config_t* msg, int op);
int xdr_player_actarray_brakes_config_t(XDR* xdrs, player_actarray_brakes_config_t* msg);
int player_actarray_brakes_config_pack(void* buf, size_t buflen, player_actarray_brakes_config_t* msg, int op);
int xdr_player_actarray_speed_config_t(XDR* xdrs, player_actarray_speed_config_t* msg);
int player_actarray_speed_config_pack(void* buf, size_t buflen, player_actarray_speed_config_t* msg, int op);
int xdr_player_aio_data_t(XDR* xdrs, player_aio_data_t* msg);
int player_aio_data_pack(void* buf, size_t buflen, player_aio_data_t* msg, int op);
int xdr_player_aio_cmd_t(XDR* xdrs, player_aio_cmd_t* msg);
int player_aio_cmd_pack(void* buf, size_t buflen, player_aio_cmd_t* msg, int op);
int xdr_player_audio_data_t(XDR* xdrs, player_audio_data_t* msg);
int player_audio_data_pack(void* buf, size_t buflen, player_audio_data_t* msg, int op);
int xdr_player_audio_cmd_t(XDR* xdrs, player_audio_cmd_t* msg);
int player_audio_cmd_pack(void* buf, size_t buflen, player_audio_cmd_t* msg, int op);
int xdr_player_audiodsp_data_t(XDR* xdrs, player_audiodsp_data_t* msg);
int player_audiodsp_data_pack(void* buf, size_t buflen, player_audiodsp_data_t* msg, int op);
int xdr_player_audiodsp_cmd_t(XDR* xdrs, player_audiodsp_cmd_t* msg);
int player_audiodsp_cmd_pack(void* buf, size_t buflen, player_audiodsp_cmd_t* msg, int op);
int xdr_player_audiodsp_config_t(XDR* xdrs, player_audiodsp_config_t* msg);
int player_audiodsp_config_pack(void* buf, size_t buflen, player_audiodsp_config_t* msg, int op);
int xdr_player_audiomixer_cmd_t(XDR* xdrs, player_audiomixer_cmd_t* msg);
int player_audiomixer_cmd_pack(void* buf, size_t buflen, player_audiomixer_cmd_t* msg, int op);
int xdr_player_audiomixer_config_t(XDR* xdrs, player_audiomixer_config_t* msg);
int player_audiomixer_config_pack(void* buf, size_t buflen, player_audiomixer_config_t* msg, int op);
int xdr_player_blinkenlight_data_t(XDR* xdrs, player_blinkenlight_data_t* msg);
int player_blinkenlight_data_pack(void* buf, size_t buflen, player_blinkenlight_data_t* msg, int op);
int xdr_player_blinkenlight_cmd_t(XDR* xdrs, player_blinkenlight_cmd_t* msg);
int player_blinkenlight_cmd_pack(void* buf, size_t buflen, player_blinkenlight_cmd_t* msg, int op);
int xdr_player_blinkenlight_cmd_power_t(XDR* xdrs, player_blinkenlight_cmd_power_t* msg);
int player_blinkenlight_cmd_power_pack(void* buf, size_t buflen, player_blinkenlight_cmd_power_t* msg, int op);
int xdr_player_blinkenlight_cmd_color_t(XDR* xdrs, player_blinkenlight_cmd_color_t* msg);
int player_blinkenlight_cmd_color_pack(void* buf, size_t buflen, player_blinkenlight_cmd_color_t* msg, int op);
int xdr_player_blinkenlight_cmd_period_t(XDR* xdrs, player_blinkenlight_cmd_period_t* msg);
int player_blinkenlight_cmd_period_pack(void* buf, size_t buflen, player_blinkenlight_cmd_period_t* msg, int op);
int xdr_player_blinkenlight_cmd_dutycycle_t(XDR* xdrs, player_blinkenlight_cmd_dutycycle_t* msg);
int player_blinkenlight_cmd_dutycycle_pack(void* buf, size_t buflen, player_blinkenlight_cmd_dutycycle_t* msg, int op);
int xdr_player_blobfinder_blob_t(XDR* xdrs, player_blobfinder_blob_t* msg);
int player_blobfinder_blob_pack(void* buf, size_t buflen, player_blobfinder_blob_t* msg, int op);
int xdr_player_blobfinder_data_t(XDR* xdrs, player_blobfinder_data_t* msg);
int player_blobfinder_data_pack(void* buf, size_t buflen, player_blobfinder_data_t* msg, int op);
int xdr_player_blobfinder_color_config_t(XDR* xdrs, player_blobfinder_color_config_t* msg);
int player_blobfinder_color_config_pack(void* buf, size_t buflen, player_blobfinder_color_config_t* msg, int op);
int xdr_player_blobfinder_imager_config_t(XDR* xdrs, player_blobfinder_imager_config_t* msg);
int player_blobfinder_imager_config_pack(void* buf, size_t buflen, player_blobfinder_imager_config_t* msg, int op);
int xdr_player_bumper_data_t(XDR* xdrs, player_bumper_data_t* msg);
int player_bumper_data_pack(void* buf, size_t buflen, player_bumper_data_t* msg, int op);
int xdr_player_bumper_define_t(XDR* xdrs, player_bumper_define_t* msg);
int player_bumper_define_pack(void* buf, size_t buflen, player_bumper_define_t* msg, int op);
int xdr_player_bumper_geom_t(XDR* xdrs, player_bumper_geom_t* msg);
int player_bumper_geom_pack(void* buf, size_t buflen, player_bumper_geom_t* msg, int op);
int xdr_player_camera_data_t(XDR* xdrs, player_camera_data_t* msg);
int player_camera_data_pack(void* buf, size_t buflen, player_camera_data_t* msg, int op);
int xdr_player_dio_data_t(XDR* xdrs, player_dio_data_t* msg);
int player_dio_data_pack(void* buf, size_t buflen, player_dio_data_t* msg, int op);
int xdr_player_dio_cmd_t(XDR* xdrs, player_dio_cmd_t* msg);
int player_dio_cmd_pack(void* buf, size_t buflen, player_dio_cmd_t* msg, int op);
int xdr_player_energy_data_t(XDR* xdrs, player_energy_data_t* msg);
int player_energy_data_pack(void* buf, size_t buflen, player_energy_data_t* msg, int op);
int xdr_player_energy_chargepolicy_config_t(XDR* xdrs, player_energy_chargepolicy_config_t* msg);
int player_energy_chargepolicy_config_pack(void* buf, size_t buflen, player_energy_chargepolicy_config_t* msg, int op);
int xdr_player_fiducial_item_t(XDR* xdrs, player_fiducial_item_t* msg);
int player_fiducial_item_pack(void* buf, size_t buflen, player_fiducial_item_t* msg, int op);
int xdr_player_fiducial_data_t(XDR* xdrs, player_fiducial_data_t* msg);
int player_fiducial_data_pack(void* buf, size_t buflen, player_fiducial_data_t* msg, int op);
int xdr_player_fiducial_geom_t(XDR* xdrs, player_fiducial_geom_t* msg);
int player_fiducial_geom_pack(void* buf, size_t buflen, player_fiducial_geom_t* msg, int op);
int xdr_player_fiducial_fov_t(XDR* xdrs, player_fiducial_fov_t* msg);
int player_fiducial_fov_pack(void* buf, size_t buflen, player_fiducial_fov_t* msg, int op);
int xdr_player_fiducial_id_t(XDR* xdrs, player_fiducial_id_t* msg);
int player_fiducial_id_pack(void* buf, size_t buflen, player_fiducial_id_t* msg, int op);
int xdr_player_gps_data_t(XDR* xdrs, player_gps_data_t* msg);
int player_gps_data_pack(void* buf, size_t buflen, player_gps_data_t* msg, int op);
int xdr_player_graphics2d_cmd_points_t(XDR* xdrs, player_graphics2d_cmd_points_t* msg);
int player_graphics2d_cmd_points_pack(void* buf, size_t buflen, player_graphics2d_cmd_points_t* msg, int op);
int xdr_player_graphics2d_cmd_polyline_t(XDR* xdrs, player_graphics2d_cmd_polyline_t* msg);
int player_graphics2d_cmd_polyline_pack(void* buf, size_t buflen, player_graphics2d_cmd_polyline_t* msg, int op);
int xdr_player_graphics2d_cmd_polygon_t(XDR* xdrs, player_graphics2d_cmd_polygon_t* msg);
int player_graphics2d_cmd_polygon_pack(void* buf, size_t buflen, player_graphics2d_cmd_polygon_t* msg, int op);
int xdr_player_graphics3d_cmd_draw_t(XDR* xdrs, player_graphics3d_cmd_draw_t* msg);
int player_graphics3d_cmd_draw_pack(void* buf, size_t buflen, player_graphics3d_cmd_draw_t* msg, int op);
int xdr_player_gripper_data_t(XDR* xdrs, player_gripper_data_t* msg);
int player_gripper_data_pack(void* buf, size_t buflen, player_gripper_data_t* msg, int op);
int xdr_player_gripper_cmd_t(XDR* xdrs, player_gripper_cmd_t* msg);
int player_gripper_cmd_pack(void* buf, size_t buflen, player_gripper_cmd_t* msg, int op);
int xdr_player_gripper_geom_t(XDR* xdrs, player_gripper_geom_t* msg);
int player_gripper_geom_pack(void* buf, size_t buflen, player_gripper_geom_t* msg, int op);
int xdr_player_ir_data_t(XDR* xdrs, player_ir_data_t* msg);
int player_ir_data_pack(void* buf, size_t buflen, player_ir_data_t* msg, int op);
int xdr_player_ir_pose_t(XDR* xdrs, player_ir_pose_t* msg);
int player_ir_pose_pack(void* buf, size_t buflen, player_ir_pose_t* msg, int op);
int xdr_player_ir_power_req_t(XDR* xdrs, player_ir_power_req_t* msg);
int player_ir_power_req_pack(void* buf, size_t buflen, player_ir_power_req_t* msg, int op);
int xdr_player_joystick_data_t(XDR* xdrs, player_joystick_data_t* msg);
int player_joystick_data_pack(void* buf, size_t buflen, player_joystick_data_t* msg, int op);
int xdr_player_laser_data_t(XDR* xdrs, player_laser_data_t* msg);
int player_laser_data_pack(void* buf, size_t buflen, player_laser_data_t* msg, int op);
int xdr_player_laser_data_scanpose_t(XDR* xdrs, player_laser_data_scanpose_t* msg);
int player_laser_data_scanpose_pack(void* buf, size_t buflen, player_laser_data_scanpose_t* msg, int op);
int xdr_player_laser_geom_t(XDR* xdrs, player_laser_geom_t* msg);
int player_laser_geom_pack(void* buf, size_t buflen, player_laser_geom_t* msg, int op);
int xdr_player_laser_config_t(XDR* xdrs, player_laser_config_t* msg);
int player_laser_config_pack(void* buf, size_t buflen, player_laser_config_t* msg, int op);
int xdr_player_laser_power_config_t(XDR* xdrs, player_laser_power_config_t* msg);
int player_laser_power_config_pack(void* buf, size_t buflen, player_laser_power_config_t* msg, int op);
int xdr_player_limb_data_t(XDR* xdrs, player_limb_data_t* msg);
int player_limb_data_pack(void* buf, size_t buflen, player_limb_data_t* msg, int op);
int xdr_player_limb_home_cmd_t(XDR* xdrs, player_limb_home_cmd_t* msg);
int player_limb_home_cmd_pack(void* buf, size_t buflen, player_limb_home_cmd_t* msg, int op);
int xdr_player_limb_stop_cmd_t(XDR* xdrs, player_limb_stop_cmd_t* msg);
int player_limb_stop_cmd_pack(void* buf, size_t buflen, player_limb_stop_cmd_t* msg, int op);
int xdr_player_limb_setpose_cmd_t(XDR* xdrs, player_limb_setpose_cmd_t* msg);
int player_limb_setpose_cmd_pack(void* buf, size_t buflen, player_limb_setpose_cmd_t* msg, int op);
int xdr_player_limb_setposition_cmd_t(XDR* xdrs, player_limb_setposition_cmd_t* msg);
int player_limb_setposition_cmd_pack(void* buf, size_t buflen, player_limb_setposition_cmd_t* msg, int op);
int xdr_player_limb_vecmove_cmd_t(XDR* xdrs, player_limb_vecmove_cmd_t* msg);
int player_limb_vecmove_cmd_pack(void* buf, size_t buflen, player_limb_vecmove_cmd_t* msg, int op);
int xdr_player_limb_power_req_t(XDR* xdrs, player_limb_power_req_t* msg);
int player_limb_power_req_pack(void* buf, size_t buflen, player_limb_power_req_t* msg, int op);
int xdr_player_limb_brakes_req_t(XDR* xdrs, player_limb_brakes_req_t* msg);
int player_limb_brakes_req_pack(void* buf, size_t buflen, player_limb_brakes_req_t* msg, int op);
int xdr_player_limb_geom_req_t(XDR* xdrs, player_limb_geom_req_t* msg);
int player_limb_geom_req_pack(void* buf, size_t buflen, player_limb_geom_req_t* msg, int op);
int xdr_player_limb_speed_req_t(XDR* xdrs, player_limb_speed_req_t* msg);
int player_limb_speed_req_pack(void* buf, size_t buflen, player_limb_speed_req_t* msg, int op);
int xdr_player_localize_hypoth_t(XDR* xdrs, player_localize_hypoth_t* msg);
int player_localize_hypoth_pack(void* buf, size_t buflen, player_localize_hypoth_t* msg, int op);
int xdr_player_localize_data_t(XDR* xdrs, player_localize_data_t* msg);
int player_localize_data_pack(void* buf, size_t buflen, player_localize_data_t* msg, int op);
int xdr_player_localize_set_pose_t(XDR* xdrs, player_localize_set_pose_t* msg);
int player_localize_set_pose_pack(void* buf, size_t buflen, player_localize_set_pose_t* msg, int op);
int xdr_player_localize_particle_t(XDR* xdrs, player_localize_particle_t* msg);
int player_localize_particle_pack(void* buf, size_t buflen, player_localize_particle_t* msg, int op);
int xdr_player_localize_get_particles_t(XDR* xdrs, player_localize_get_particles_t* msg);
int player_localize_get_particles_pack(void* buf, size_t buflen, player_localize_get_particles_t* msg, int op);
int xdr_player_log_set_write_state_t(XDR* xdrs, player_log_set_write_state_t* msg);
int player_log_set_write_state_pack(void* buf, size_t buflen, player_log_set_write_state_t* msg, int op);
int xdr_player_log_set_read_state_t(XDR* xdrs, player_log_set_read_state_t* msg);
int player_log_set_read_state_pack(void* buf, size_t buflen, player_log_set_read_state_t* msg, int op);
int xdr_player_log_set_read_rewind_t(XDR* xdrs, player_log_set_read_rewind_t* msg);
int player_log_set_read_rewind_pack(void* buf, size_t buflen, player_log_set_read_rewind_t* msg, int op);
int xdr_player_log_get_state_t(XDR* xdrs, player_log_get_state_t* msg);
int player_log_get_state_pack(void* buf, size_t buflen, player_log_get_state_t* msg, int op);
int xdr_player_log_set_filename_t(XDR* xdrs, player_log_set_filename_t* msg);
int player_log_set_filename_pack(void* buf, size_t buflen, player_log_set_filename_t* msg, int op);
int xdr_player_map_info_t(XDR* xdrs, player_map_info_t* msg);
int player_map_info_pack(void* buf, size_t buflen, player_map_info_t* msg, int op);
int xdr_player_map_data_t(XDR* xdrs, player_map_data_t* msg);
int player_map_data_pack(void* buf, size_t buflen, player_map_data_t* msg, int op);
int xdr_player_map_data_vector_t(XDR* xdrs, player_map_data_vector_t* msg);
int player_map_data_vector_pack(void* buf, size_t buflen, player_map_data_vector_t* msg, int op);
int xdr_player_mcom_data_t(XDR* xdrs, player_mcom_data_t* msg);
int player_mcom_data_pack(void* buf, size_t buflen, player_mcom_data_t* msg, int op);
int xdr_player_mcom_config_t(XDR* xdrs, player_mcom_config_t* msg);
int player_mcom_config_pack(void* buf, size_t buflen, player_mcom_config_t* msg, int op);
int xdr_player_mcom_return_t(XDR* xdrs, player_mcom_return_t* msg);
int player_mcom_return_pack(void* buf, size_t buflen, player_mcom_return_t* msg, int op);
int xdr_player_opaque_data_t(XDR* xdrs, player_opaque_data_t* msg);
int player_opaque_data_pack(void* buf, size_t buflen, player_opaque_data_t* msg, int op);
int xdr_player_planner_data_t(XDR* xdrs, player_planner_data_t* msg);
int player_planner_data_pack(void* buf, size_t buflen, player_planner_data_t* msg, int op);
int xdr_player_planner_cmd_t(XDR* xdrs, player_planner_cmd_t* msg);
int player_planner_cmd_pack(void* buf, size_t buflen, player_planner_cmd_t* msg, int op);
int xdr_player_planner_waypoints_req_t(XDR* xdrs, player_planner_waypoints_req_t* msg);
int player_planner_waypoints_req_pack(void* buf, size_t buflen, player_planner_waypoints_req_t* msg, int op);
int xdr_player_planner_enable_req_t(XDR* xdrs, player_planner_enable_req_t* msg);
int player_planner_enable_req_pack(void* buf, size_t buflen, player_planner_enable_req_t* msg, int op);
int xdr_player_device_devlist_t(XDR* xdrs, player_device_devlist_t* msg);
int player_device_devlist_pack(void* buf, size_t buflen, player_device_devlist_t* msg, int op);
int xdr_player_device_driverinfo_t(XDR* xdrs, player_device_driverinfo_t* msg);
int player_device_driverinfo_pack(void* buf, size_t buflen, player_device_driverinfo_t* msg, int op);
int xdr_player_device_req_t(XDR* xdrs, player_device_req_t* msg);
int player_device_req_pack(void* buf, size_t buflen, player_device_req_t* msg, int op);
int xdr_player_device_data_req_t(XDR* xdrs, player_device_data_req_t* msg);
int player_device_data_req_pack(void* buf, size_t buflen, player_device_data_req_t* msg, int op);
int xdr_player_device_datamode_req_t(XDR* xdrs, player_device_datamode_req_t* msg);
int player_device_datamode_req_pack(void* buf, size_t buflen, player_device_datamode_req_t* msg, int op);
int xdr_player_device_auth_req_t(XDR* xdrs, player_device_auth_req_t* msg);
int player_device_auth_req_pack(void* buf, size_t buflen, player_device_auth_req_t* msg, int op);
int xdr_player_device_nameservice_req_t(XDR* xdrs, player_device_nameservice_req_t* msg);
int player_device_nameservice_req_pack(void* buf, size_t buflen, player_device_nameservice_req_t* msg, int op);
int xdr_player_add_replace_rule_req_t(XDR* xdrs, player_add_replace_rule_req_t* msg);
int player_add_replace_rule_req_pack(void* buf, size_t buflen, player_add_replace_rule_req_t* msg, int op);
int xdr_player_position1d_data_t(XDR* xdrs, player_position1d_data_t* msg);
int player_position1d_data_pack(void* buf, size_t buflen, player_position1d_data_t* msg, int op);
int xdr_player_position1d_cmd_vel_t(XDR* xdrs, player_position1d_cmd_vel_t* msg);
int player_position1d_cmd_vel_pack(void* buf, size_t buflen, player_position1d_cmd_vel_t* msg, int op);
int xdr_player_position1d_cmd_pos_t(XDR* xdrs, player_position1d_cmd_pos_t* msg);
int player_position1d_cmd_pos_pack(void* buf, size_t buflen, player_position1d_cmd_pos_t* msg, int op);
int xdr_player_position1d_geom_t(XDR* xdrs, player_position1d_geom_t* msg);
int player_position1d_geom_pack(void* buf, size_t buflen, player_position1d_geom_t* msg, int op);
int xdr_player_position1d_power_config_t(XDR* xdrs, player_position1d_power_config_t* msg);
int player_position1d_power_config_pack(void* buf, size_t buflen, player_position1d_power_config_t* msg, int op);
int xdr_player_position1d_velocity_mode_config_t(XDR* xdrs, player_position1d_velocity_mode_config_t* msg);
int player_position1d_velocity_mode_config_pack(void* buf, size_t buflen, player_position1d_velocity_mode_config_t* msg, int op);
int xdr_player_position1d_reset_odom_config_t(XDR* xdrs, player_position1d_reset_odom_config_t* msg);
int player_position1d_reset_odom_config_pack(void* buf, size_t buflen, player_position1d_reset_odom_config_t* msg, int op);
int xdr_player_position1d_position_mode_req_t(XDR* xdrs, player_position1d_position_mode_req_t* msg);
int player_position1d_position_mode_pack(void* buf, size_t buflen, player_position1d_position_mode_req_t* msg, int op);
int xdr_player_position1d_set_odom_req_t(XDR* xdrs, player_position1d_set_odom_req_t* msg);
int player_position1d_set_odom_pack(void* buf, size_t buflen, player_position1d_set_odom_req_t* msg, int op);
int xdr_player_position1d_speed_pid_req_t(XDR* xdrs, player_position1d_speed_pid_req_t* msg);
int player_position1d_speed_pid_pack(void* buf, size_t buflen, player_position1d_speed_pid_req_t* msg, int op);
int xdr_player_position1d_position_pid_req_t(XDR* xdrs, player_position1d_position_pid_req_t* msg);
int player_position1d_position_pid_pack(void* buf, size_t buflen, player_position1d_position_pid_req_t* msg, int op);
int xdr_player_position1d_speed_prof_req_t(XDR* xdrs, player_position1d_speed_prof_req_t* msg);
int player_position1d_speed_prof_pack(void* buf, size_t buflen, player_position1d_speed_prof_req_t* msg, int op);
int xdr_player_position2d_data_t(XDR* xdrs, player_position2d_data_t* msg);
int player_position2d_data_pack(void* buf, size_t buflen, player_position2d_data_t* msg, int op);
int xdr_player_position2d_cmd_vel_t(XDR* xdrs, player_position2d_cmd_vel_t* msg);
int player_position2d_cmd_vel_pack(void* buf, size_t buflen, player_position2d_cmd_vel_t* msg, int op);
int xdr_player_position2d_cmd_pos_t(XDR* xdrs, player_position2d_cmd_pos_t* msg);
int player_position2d_cmd_pos_pack(void* buf, size_t buflen, player_position2d_cmd_pos_t* msg, int op);
int xdr_player_position2d_cmd_car_t(XDR* xdrs, player_position2d_cmd_car_t* msg);
int player_position2d_cmd_car_pack(void* buf, size_t buflen, player_position2d_cmd_car_t* msg, int op);
int xdr_player_position2d_geom_t(XDR* xdrs, player_position2d_geom_t* msg);
int player_position2d_geom_pack(void* buf, size_t buflen, player_position2d_geom_t* msg, int op);
int xdr_player_position2d_power_config_t(XDR* xdrs, player_position2d_power_config_t* msg);
int player_position2d_power_config_pack(void* buf, size_t buflen, player_position2d_power_config_t* msg, int op);
int xdr_player_position2d_velocity_mode_config_t(XDR* xdrs, player_position2d_velocity_mode_config_t* msg);
int player_position2d_velocity_mode_config_pack(void* buf, size_t buflen, player_position2d_velocity_mode_config_t* msg, int op);
int xdr_player_position2d_reset_odom_config_t(XDR* xdrs, player_position2d_reset_odom_config_t* msg);
int player_position2d_reset_odom_config_pack(void* buf, size_t buflen, player_position2d_reset_odom_config_t* msg, int op);
int xdr_player_position2d_position_mode_req_t(XDR* xdrs, player_position2d_position_mode_req_t* msg);
int player_position2d_position_mode_req_pack(void* buf, size_t buflen, player_position2d_position_mode_req_t* msg, int op);
int xdr_player_position2d_set_odom_req_t(XDR* xdrs, player_position2d_set_odom_req_t* msg);
int player_position2d_set_odom_req_pack(void* buf, size_t buflen, player_position2d_set_odom_req_t* msg, int op);
int xdr_player_position2d_speed_pid_req_t(XDR* xdrs, player_position2d_speed_pid_req_t* msg);
int player_position2d_speed_pid_req_pack(void* buf, size_t buflen, player_position2d_speed_pid_req_t* msg, int op);
int xdr_player_position2d_position_pid_req_t(XDR* xdrs, player_position2d_position_pid_req_t* msg);
int player_position2d_position_pid_req_pack(void* buf, size_t buflen, player_position2d_position_pid_req_t* msg, int op);
int xdr_player_position2d_speed_prof_req_t(XDR* xdrs, player_position2d_speed_prof_req_t* msg);
int player_position2d_speed_prof_req_pack(void* buf, size_t buflen, player_position2d_speed_prof_req_t* msg, int op);
int xdr_player_position3d_data_t(XDR* xdrs, player_position3d_data_t* msg);
int player_position3d_data_pack(void* buf, size_t buflen, player_position3d_data_t* msg, int op);
int xdr_player_position3d_cmd_pos_t(XDR* xdrs, player_position3d_cmd_pos_t* msg);
int player_position3d_cmd_pos_pack(void* buf, size_t buflen, player_position3d_cmd_pos_t* msg, int op);
int xdr_player_position3d_cmd_vel_t(XDR* xdrs, player_position3d_cmd_vel_t* msg);
int player_position3d_cmd_vel_pack(void* buf, size_t buflen, player_position3d_cmd_vel_t* msg, int op);
int xdr_player_position3d_geom_t(XDR* xdrs, player_position3d_geom_t* msg);
int player_position3d_geom_pack(void* buf, size_t buflen, player_position3d_geom_t* msg, int op);
int xdr_player_position3d_power_config_t(XDR* xdrs, player_position3d_power_config_t* msg);
int player_position3d_power_config_pack(void* buf, size_t buflen, player_position3d_power_config_t* msg, int op);
int xdr_player_position3d_position_mode_req_t(XDR* xdrs, player_position3d_position_mode_req_t* msg);
int player_position3d_position_mode_req_pack(void* buf, size_t buflen, player_position3d_position_mode_req_t* msg, int op);
int xdr_player_position3d_velocity_mode_config_t(XDR* xdrs, player_position3d_velocity_mode_config_t* msg);
int player_position3d_velocity_mode_config_pack(void* buf, size_t buflen, player_position3d_velocity_mode_config_t* msg, int op);
int xdr_player_position3d_set_odom_req_t(XDR* xdrs, player_position3d_set_odom_req_t* msg);
int player_position3d_set_odom_req_pack(void* buf, size_t buflen, player_position3d_set_odom_req_t* msg, int op);
int xdr_player_position3d_reset_odom_config_t(XDR* xdrs, player_position3d_reset_odom_config_t* msg);
int player_position3d_reset_odom_config_pack(void* buf, size_t buflen, player_position3d_reset_odom_config_t* msg, int op);
int xdr_player_position3d_speed_pid_req_t(XDR* xdrs, player_position3d_speed_pid_req_t* msg);
int player_position3d_speed_pid_req_pack(void* buf, size_t buflen, player_position3d_speed_pid_req_t* msg, int op);
int xdr_player_position3d_position_pid_req_t(XDR* xdrs, player_position3d_position_pid_req_t* msg);
int player_position3d_position_pid_req_pack(void* buf, size_t buflen, player_position3d_position_pid_req_t* msg, int op);
int xdr_player_position3d_speed_prof_req_t(XDR* xdrs, player_position3d_speed_prof_req_t* msg);
int player_position3d_speed_prof_req_pack(void* buf, size_t buflen, player_position3d_speed_prof_req_t* msg, int op);
int xdr_player_power_data_t(XDR* xdrs, player_power_data_t* msg);
int player_power_data_pack(void* buf, size_t buflen, player_power_data_t* msg, int op);
int xdr_player_power_chargepolicy_config_t(XDR* xdrs, player_power_chargepolicy_config_t* msg);
int player_power_chargepolicy_config_pack(void* buf, size_t buflen, player_power_chargepolicy_config_t* msg, int op);
int xdr_player_ptz_data_t(XDR* xdrs, player_ptz_data_t* msg);
int player_ptz_data_pack(void* buf, size_t buflen, player_ptz_data_t* msg, int op);
int xdr_player_ptz_cmd_t(XDR* xdrs, player_ptz_cmd_t* msg);
int player_ptz_cmd_pack(void* buf, size_t buflen, player_ptz_cmd_t* msg, int op);
int xdr_player_ptz_geom_t(XDR* xdrs, player_ptz_geom_t* msg);
int player_ptz_geom_pack(void* buf, size_t buflen, player_ptz_geom_t* msg, int op);
int xdr_player_ptz_req_generic_t(XDR* xdrs, player_ptz_req_generic_t* msg);
int player_ptz_req_generic_pack(void* buf, size_t buflen, player_ptz_req_generic_t* msg, int op);
int xdr_player_ptz_req_control_mode_t(XDR* xdrs, player_ptz_req_control_mode_t* msg);
int player_ptz_req_control_mode_pack(void* buf, size_t buflen, player_ptz_req_control_mode_t* msg, int op);
int xdr_player_simulation_data_t(XDR* xdrs, player_simulation_data_t* msg);
int player_simulation_data_pack(void* buf, size_t buflen, player_simulation_data_t* msg, int op);
int xdr_player_simulation_cmd_t(XDR* xdrs, player_simulation_cmd_t* msg);
int player_simulation_cmd_pack(void* buf, size_t buflen, player_simulation_cmd_t* msg, int op);
int xdr_player_simulation_pose2d_req_t(XDR* xdrs, player_simulation_pose2d_req_t* msg);
int player_simulation_pose2d_req_pack(void* buf, size_t buflen, player_simulation_pose2d_req_t* msg, int op);
int xdr_player_simulation_property_int_req_t(XDR* xdrs, player_simulation_property_int_req_t* msg);
int player_simulation_property_int_req_pack(void* buf, size_t buflen, player_simulation_property_int_req_t* msg, int op);
int xdr_player_simulation_property_float_req_t(XDR* xdrs, player_simulation_property_float_req_t* msg);
int player_simulation_property_float_req_pack(void* buf, size_t buflen, player_simulation_property_float_req_t* msg, int op);
int xdr_player_simulation_property_string_req_t(XDR* xdrs, player_simulation_property_string_req_t* msg);
int player_simulation_property_string_req_pack(void* buf, size_t buflen, player_simulation_property_string_req_t* msg, int op);
int xdr_player_sonar_data_t(XDR* xdrs, player_sonar_data_t* msg);
int player_sonar_data_pack(void* buf, size_t buflen, player_sonar_data_t* msg, int op);
int xdr_player_sonar_geom_t(XDR* xdrs, player_sonar_geom_t* msg);
int player_sonar_geom_pack(void* buf, size_t buflen, player_sonar_geom_t* msg, int op);
int xdr_player_sonar_power_config_t(XDR* xdrs, player_sonar_power_config_t* msg);
int player_sonar_power_config_pack(void* buf, size_t buflen, player_sonar_power_config_t* msg, int op);
int xdr_player_sound_cmd_t(XDR* xdrs, player_sound_cmd_t* msg);
int player_sound_cmd_pack(void* buf, size_t buflen, player_sound_cmd_t* msg, int op);
int xdr_player_speech_cmd_t(XDR* xdrs, player_speech_cmd_t* msg);
int player_speech_cmd_pack(void* buf, size_t buflen, player_speech_cmd_t* msg, int op);
int xdr_player_speech_recognition_data_t(XDR* xdrs, player_speech_recognition_data_t* msg);
int player_speech_recognition_data_pack(void* buf, size_t buflen, player_speech_recognition_data_t* msg, int op);
int xdr_player_truth_pose_t(XDR* xdrs, player_truth_pose_t* msg);
int player_truth_pose_pack(void* buf, size_t buflen, player_truth_pose_t* msg, int op);
int xdr_player_truth_fiducial_id_t(XDR* xdrs, player_truth_fiducial_id_t* msg);
int player_truth_fiducial_id_pack(void* buf, size_t buflen, player_truth_fiducial_id_t* msg, int op);
int xdr_player_waveform_data_t(XDR* xdrs, player_waveform_data_t* msg);
int player_waveform_data_pack(void* buf, size_t buflen, player_waveform_data_t* msg, int op);
int xdr_player_wifi_link_t(XDR* xdrs, player_wifi_link_t* msg);
int player_wifi_link_pack(void* buf, size_t buflen, player_wifi_link_t* msg, int op);
int xdr_player_wifi_data_t(XDR* xdrs, player_wifi_data_t* msg);
int player_wifi_data_pack(void* buf, size_t buflen, player_wifi_data_t* msg, int op);
int xdr_player_wifi_mac_req_t(XDR* xdrs, player_wifi_mac_req_t* msg);
int player_wifi_mac_req_pack(void* buf, size_t buflen, player_wifi_mac_req_t* msg, int op);
int xdr_player_wifi_iwspy_addr_req_t(XDR* xdrs, player_wifi_iwspy_addr_req_t* msg);
int player_wifi_iwspy_addr_req_pack(void* buf, size_t buflen, player_wifi_iwspy_addr_req_t* msg, int op);
int xdr_player_rfid_tag_t(XDR* xdrs, player_rfid_tag_t* msg);
int player_rfid_tag_pack(void* buf, size_t buflen, player_rfid_tag_t* msg, int op);
int xdr_player_rfid_data_t(XDR* xdrs, player_rfid_data_t* msg);
int player_rfid_data_pack(void* buf, size_t buflen, player_rfid_data_t* msg, int op);
int xdr_player_rfid_cmd_t(XDR* xdrs, player_rfid_cmd_t* msg);
int player_rfid_cmd_pack(void* buf, size_t buflen, player_rfid_cmd_t* msg, int op);
int xdr_player_wsn_node_data_t(XDR* xdrs, player_wsn_node_data_t* msg);
int player_wsn_node_data_pack(void* buf, size_t buflen, player_wsn_node_data_t* msg, int op);
int xdr_player_wsn_data_t(XDR* xdrs, player_wsn_data_t* msg);
int player_wsn_data_pack(void* buf, size_t buflen, player_wsn_data_t* msg, int op);
int xdr_player_wsn_cmd_t(XDR* xdrs, player_wsn_cmd_t* msg);
int player_wsn_cmd_pack(void* buf, size_t buflen, player_wsn_cmd_t* msg, int op);
int xdr_player_wsn_power_config_t(XDR* xdrs, player_wsn_power_config_t* msg);
int player_wsn_power_config_pack(void* buf, size_t buflen, player_wsn_power_config_t* msg, int op);
int xdr_player_wsn_datatype_config_t(XDR* xdrs, player_wsn_datatype_config_t* msg);
int player_wsn_datatype_config_pack(void* buf, size_t buflen, player_wsn_datatype_config_t* msg, int op);
int xdr_player_wsn_datafreq_config_t(XDR* xdrs, player_wsn_datafreq_config_t* msg);
int player_wsn_datafreq_config_pack(void* buf, size_t buflen, player_wsn_datafreq_config_t* msg, int op);

#ifdef __cplusplus
}
#endif

#endif
/** @} */
