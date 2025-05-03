# ROCF

**Detecting Lifecycle-Related Concurrency Bugs in ROS Programs via Coverage-Guided Fuzzing**

## Experimental Environment

* **Operating System**: Ubuntu 22.04 or later
* **Robot Operating System (ROS)**: ROS 2 Humble or later
* **Physical Memory**: 64 GB

**Note**: ROCF is verified to support testing ROS programs across any ROS 2 version.

## Usage

1. **Update and Upgrade System Packages**:
    ```bash
    sudo apt-get update  
    sudo apt-get upgrade  
    ```

2. **Set Up Environment**:
    * Correctly build ROS 2 Humble.
    * Correctly build your target ROS programs.

3. **Edit Configuration File**:

   Modify the configuration file located at `bin/fuzzer_config.yaml` to suit your testing needs.

4. **Run the Fuzzer**:

   Execute the following commands:

   ```bash
   cd bin/  
   ./fuzzer_launch.sh  
   ```

5. **Review Logs**:

   The logs will be saved in the `bin/fuzz_log` folder, which is automatically created by ROCF.


# Found Lifecycle-Related Concurrency Bugs
- BFO means BuFfer Overflow
- UAF means Use After Free
- NPD means Null Pointer Dereference

| **ID** | **Program**        | **Bug type** | **Bug triggering location**               | **Related lifecycle** | **CVE ID**     | **CVSS score** |
|--------|--------------------|--------------|-------------------------------------------|-----------------------|----------------|----------------|
| 1      | nav2_amcl          | UAF          | amcl_node.cpp:1167                        | Cleaningup            | CVE-2024-41644 | Critical, 9.8  |
| 2      | nav2_amcl          | UAF          | pf.c:150                                  | Cleaningup            | CVE-2024-30964 | High, 7.8      |
| 3      | nav2_amcl          | UAF          | amcl_node.cpp:1439                        | Cleaningup            | CVE-2024-25198 | Critical, 9.1  |
| 4      | nav2_amcl          | UAF          | amcl_node.cpp:1418                        | Cleaningup            | CVE-2024-25199 | High, 8.1      |
| 5      | nav2_amcl          | BFO          | pf.c:293                                  | Configuring           | CVE-2024-30963 | High, 7.8      |
| 6      | nav2_bt_navigator  | UAF          | navigator.hpp:90                          | Deactivating          | CVE-2024-30961 | High, 7.8      |
| 7      | nav2_controller    | NPD          | robot_utils.cpp:38                        | Cleaningup            | CVE-2024-41649 | Critical, 9.8  |
| 8      | nav2_controller    | NPD          | controller_server.cpp:781                 | Cleaningup            | -              | -              |
| 9      | nav2_controller    | UAF          | inflation_layer.cpp:392                   | Cleaningup            | CVE-2024-41650 | Critical, 9.8  |
| 10     | nav2_controller    | UAF          | inflation_layer.cpp:436                   | Cleaningup            | CVE-2024-41645 | Critical, 9.8  |
| 11     | nav2_controller    | NPD          | dwb_local_planner.cpp:252                 | Shuttingdown          | CVE-2024-41646 | Critical, 9.8  |
| 12     | nav2_controller    | NPD          | controller.cpp:77                         | Shuttingdown          | CVE-2024-41647 | Critical, 9.8  |
| 13     | nav2_controller    | NPD          | regulated_pure_pursuit_controller.cpp:154 | Shuttingdown          | CVE-2024-41648 | Critical, 9.8  |
| 14     | nav2_controller    | NPD          | layered_costmap.cpp:260                   | Shuttingdown          | CVE-2024-25197 | Medium, 6.5    |
| 15     | nav2_controller    | NPD          | controller_server.cpp:429                 | Deactivating          | CVE-2024-44853 | High, 7.5      |
| 16     | nav2_planner       | NPD          | robot_utils.cpp:38                        | Cleaningup            | -              | -              |
| 17     | nav2_planner       | UAF          | inflation_layer.cpp:392                   | Cleaningup            | -              | -              |
| 18     | nav2_planner       | UAF          | inflation_layer.cpp:436                   | Cleaningup            | -              | -              |
| 19     | nav2_planner       | BFO          | inflation_layer.cpp:131                   | Configuring           | CVE-2024-37862 | High, 7.3      |
| 20     | nav2_planner       | NPD          | theta_star.hpp:116                        | Shuttingdown          | CVE-2024-44855 | High, 7.5      |
| 21     | nav2_planner       | NPD          | utils.hpp:75                              | Shuttingdown          | CVE-2024-44856 | High, 7.5      |
| 22     | nav2_planner       | BFO          | navfn_planner.cpp:219                     | Shuttingdown          | CVE-2024-44852 | Critical, 9.8  |
| 23     | nav2_planner       | UAF          | layered_costmap.cpp:260                   | Shuttingdown          | -              | -              |
| 24     | nav2_smoother      | NPD          | nav2_smoother.cpp:268                     | Deactivating          | CVE-2024-44854 | High, 7.5      |
| 25     | slam_toolbox_async | NPD          | slam_toolbox_common.cpp:501               | Cleaningup            | -              | -              |
| 26     | slam_toolbox_async | NPD          | slam_mapper.cpp:67                        | Cleaningup            | -              | -              |
| 27     | slam_toolbox_async | UAF          | slam_toolbox_async.cpp:34                 | Cleaningup            | -              | -              |
| 28     | slam_toolbox_async | UAF          | slam_toolbox_async.cpp:56                 | Cleaningup            | -              | -              |
| 29     | slam_toolbox_sync  | NPD          | slam_toolbox_common.cpp:501               | Cleaningup            | -              | -              |
| 30     | slam_toolbox_sync  | NPD          | slam_mapper.cpp:67                        | Cleaningup            | -              | -              |
| 31     | slam_toolbox_sync  | UAF          | slam_toolbox_sync.cpp:73                  | Cleaningup            | -              | -              |
| 32     | slam_toolbox_sync  | UAF          | slam_toolbox_sync.cpp:96                  | Cleaningup            | -              | -              |
