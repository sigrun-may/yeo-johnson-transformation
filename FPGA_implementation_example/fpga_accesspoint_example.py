# Copyright (c) 2023 Jerome Brenig, Sigrun May, Ostfalia Hochschule für angewandte Wissenschaften
# This software is distributed under the terms of the MIT license
# which is available at https://opensource.org/licenses/MIT

import numpy as np
import paramiko as pm

def init_fpga():
    try:
        ssh = pm.SSHClient()
        ssh.connect(hostname='192.168.1.0', port=22, username="name", password="password")
        ssh_stdin, ssh_stdout, ssh_stderr = ssh.exec_command('sudo su')
        print("ssh_stdin: ", ssh_stdin)
        print("ssh_stdout: ", ssh_stdout)
        print("ssh_stderr: ", ssh_stderr)
        ssh.exec_command('echo version05.bin > /sys/class/fpga_manager/fpga0/firmware')
        print('FPGA Initialized')
    finally:
        if ssh:
            ssh.close

def yeo_johnson_fpga_interface(
        unlabeled_data_np: np.ndarray,
        precision_parameter: int,
        ):
    # Getting dimensions of input array
    column_dimension = np.size(unlabeled_data_np, 1)
    row_dimension = np.size(unlabeled_data_np, 0)
    # Creating one dimensionl array
    length = column_dimension * row_dimension
    output_array = np.zeros(shape=[length], dtype=float)
    # Filling array
    indicator = 0
    for i in range(column_dimension):
        for j in range(row_dimension):
            output_array[indicator] = unlabeled_data_np[j][i]
            indicator += 1
    # Writing file for fpga access
    output_array.astype(np.float32).tofile("/data/storage/user/INPUT.bin")
    # Connecting to node
    try:
        ssh = pm.SSHClient()
        ssh.connect(hostname='192.168.1.0', port=22, username="name", password="password")
        ssh_stdin, ssh_stdout, ssh_stderr = ssh.exec_command('sudo su')
        # do something with outputs?
        print("ssh_stdin: ", ssh_stdin)
        print("ssh_stdout: ", ssh_stdout)
        print("ssh_stderr: ", ssh_stderr)
        command = "./main {rows:d} {size_of_stream:d} {precision:d}"
        # Executing fpga script
        command.format(rows=row_dimension, size_of_stream=column_dimension, precision=precision_parameter)
        ssh_stdin, ssh_stdout, ssh_stderr = ssh.exec_command(command)
        # do something with outputs?
        print("ssh_stdin: ", ssh_stdin)
        print("ssh_stdout: ", ssh_stdout)
        print("ssh_stderr: ", ssh_stderr)
    finally:
        if ssh:
            # leave ssh connection
            ssh.close
    # Reading results from file
    with open('/data/storage/user/fpga_data/OUTPUT.bin', 'rb') as f:
        b = f.read()
    lambdas_np_data = np.frombuffer(b, float)
    return lambdas_np_data
