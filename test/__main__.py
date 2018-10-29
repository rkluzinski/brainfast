import subprocess

command = "build/bf test/{0}.b"

programs = [
    "bitwidth"
    ]

def main():
    for program in programs:
        print("Running {}.b: ".format(program), end="", flush=True)
        subprocess.call(command.format(program), shell=True)


if __name__ == "__main__":
    main()
