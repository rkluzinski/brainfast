import subprocess
from time import time
from resource import getrusage, RUSAGE_CHILDREN


command = "./brainfast benchmarks/{0}.b < benchmarks/{0}.in > benchmarks/{0}.out"

programs = [
    "awib-0.4",
    "dbfi",
    "factor",
    "hanoi",
    "long",
    "mandelbrot",
    ]


def time_process(program, stdin=None):
    # start time
    ru_start = getrusage(RUSAGE_CHILDREN)
    start = time()

    # run process
    subprocess.call(command.format(program), shell=True)

    # end time
    end = time();
    ru_end = getrusage(RUSAGE_CHILDREN)

    # compute elapsed
    real = end - start
    user = ru_end.ru_utime - ru_start.ru_utime
    system = ru_end.ru_stime - ru_start.ru_stime

    return (real, user, system)


def main():
    # store the total value
    total = (0, 0, 0)

    # print the header
    print("{:20s} {:>8s} {:>8s} {:>8s}".format("program", "real", "user", "sys"))
    print("-" * 48)

    # for each program
    for program in programs:
        
        # get the real, user, and system time
        real, user, system = time_process(program)
        total = tuple(map(lambda x,y: x+y, total, (real, user, system)))
        print("{:20s} {:8.3f} {:8.3f} {:8.3f}".format(program, real, user, system))

    # print total
    print("{:20s} {:8.3f} {:8.3f} {:8.3f}".format("total", *total))


if __name__ == "__main__":
    main()
