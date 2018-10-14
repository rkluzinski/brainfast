import subprocess
from time import time
from resource import getrusage, RUSAGE_SELF, RUSAGE_CHILDREN


command = "time ./brainfast benchmarks/{}"

programs = [
    #"factor.b",
    "hanoi.b",
    "long.b",
    "mandelbrot.b",
    ]


def time_process(program, stdin=None):
    # start time
    ru_start = getrusage(RUSAGE_CHILDREN)
    start = time()

    # run process
    process = subprocess.Popen(
        command.format(program),
        #stdin=b'137',
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        shell=True)

    process.wait()

    # end time
    end = time();
    ru_end = getrusage(RUSAGE_CHILDREN)

    # compute elapsed
    real = end - start
    user = ru_end.ru_utime - ru_start.ru_utime
    system = ru_end.ru_stime - ru_start.ru_stime

    return (real, user, system)


def main():
    print("{:20s} {:>8s} {:>8s} {:>8s}".format("program", "real", "user", "sys"))
    print("-" * 48)
    for program in programs:
        real, user, system = time_process(program)
        print("{:20s} {:8.3f} {:8.3f} {:8.3f}".format(program, real, user, system))


if __name__ == "__main__":
    main()
