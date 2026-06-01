import argparse
import os
import random
import shlex
import signal
import subprocess
import time
from pathlib import Path


RANDOM_LIMIT = 1000
SEED = 123456789
random.seed(SEED)

AMMUNITION = [
    'localhost:8080/api/v1/maps/map1',
    'localhost:8080/api/v1/maps'
]

SHOOT_COUNT = 100
COOLDOWN = 0.1


def start_server():
    parser = argparse.ArgumentParser()
    parser.add_argument('server', type=str)
    return parser.parse_args().server


def run(command, output=None):
    return subprocess.Popen(
        shlex.split(command),
        stdout=output,
        stderr=subprocess.DEVNULL
    )


def stop(process, wait=False):
    if process.poll() is not None:
        return

    process.terminate()

    if wait:
        try:
            process.wait(timeout=5)
        except subprocess.TimeoutExpired:
            process.kill()
            process.wait()


def shoot(ammo):
    hit = run('curl ' + ammo, output=subprocess.DEVNULL)
    time.sleep(COOLDOWN)
    stop(hit, wait=True)


def make_shots():
    for _ in range(SHOOT_COUNT):
        ammo_number = random.randrange(RANDOM_LIMIT) % len(AMMUNITION)
        shoot(AMMUNITION[ammo_number])

    print('Shooting complete')


def start_perf(server_process):
    return subprocess.Popen(
        [
            'perf',
            'record',
            '-o',
            'perf.data',
            '-g',
            '-p',
            str(server_process.pid)
        ],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL
    )


def stop_perf(perf_process):
    if perf_process.poll() is not None:
        return

    perf_process.send_signal(signal.SIGINT)

    try:
        perf_process.wait(timeout=10)
    except subprocess.TimeoutExpired:
        perf_process.terminate()
        try:
            perf_process.wait(timeout=5)
        except subprocess.TimeoutExpired:
            perf_process.kill()
            perf_process.wait()


def build_flamegraph():
    script_dir = Path(__file__).resolve().parent
    flamegraph_dir = script_dir / 'FlameGraph'

    stackcollapse = flamegraph_dir / 'stackcollapse-perf.pl'
    flamegraph = flamegraph_dir / 'flamegraph.pl'

    with open('folded.stacks', 'w') as folded:
        perf_script = subprocess.Popen(
            ['perf', 'script', '-i', 'perf.data'],
            stdout=subprocess.PIPE,
            stderr=subprocess.DEVNULL
        )

        collapse = subprocess.Popen(
            [str(stackcollapse)],
            stdin=perf_script.stdout,
            stdout=folded,
            stderr=subprocess.DEVNULL
        )

        perf_script.stdout.close()
        collapse.wait()
        perf_script.wait()

    with open('graph.svg', 'w') as graph:
        subprocess.run(
            [str(flamegraph), 'folded.stacks'],
            stdout=graph,
            stderr=subprocess.DEVNULL,
            check=True
        )


server_command = start_server()

server = run(server_command)

time.sleep(1)

perf = start_perf(server)

time.sleep(1)

make_shots()

stop_perf(perf)

stop(server, wait=True)

build_flamegraph()

print('Job done')
