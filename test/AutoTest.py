"""
This script ...
usage:
    ./myscript.py
"""

import argparse
import logging
from time import sleep, localtime, strftime
import filecmp
import os
import re
import sys
import subprocess
from shutil import which


parser = argparse.ArgumentParser()
parser.add_argument(
    "--logging",
    default="info",
    choices=["debug", "info", "warning", "error", "critical"],
    help="Logging level",
)

parser.add_argument(
    "--elf",
    help="The test program",
)

parser.add_argument(
    "--show-diff",
    default=False,
    help="In case of error show the diff",
)


def main(args):
    logging.info("+{:->84}".format("+"))
    logging.info("|{: ^83}|".format("STARTING AUTO TEST"))
    logging.info("+{:->84}".format("+"))

    at = AutoTest(
        str(args.elf),
        inDir="test/mock/",
        outDir="build/x86/",
        expectDir="test/mock/expected/",
    )

    testList = []

    testList.append(
        TestFeature(
            name="Bootloader success",
            scenario=[
                TestScenario(
                    cmd="ProgramMemoryLoad", input="FW_1.txt", expected="FW_1_Ok.txt"
                )
            ],
        )
    )
    testList.append(
        TestFeature(
            name="Bootloader CRC error",
            scenario=[
                TestScenario(
                    cmd="ProgramMemoryLoad",
                    input="FW_crcError.txt",
                    expected="FW_crcError.txt",
                )
            ],
        )
    )
    testList.append(
        TestFeature(
            name="Bootloader alingment error",
            scenario=[
                TestScenario(
                    cmd="ProgramMemoryLoad",
                    input="FW_algmtError.txt",
                    expected="FW_algmtError.txt",
                )
            ],
        )
    )
    testList.append(
        TestFeature(
            name="Bootloader overflow error",
            scenario=[
                TestScenario(
                    cmd="ProgramMemoryLoad",
                    input="FW_overflowError.txt",
                    expected="FW_overflowError.txt",
                )
            ],
        )
    )
    testList.append(
        TestFeature(
            name="Bootloader invalid address",
            scenario=[
                TestScenario(
                    cmd="ProgramMemoryLoad",
                    input="FW_invalidAddr.txt",
                    expected="FW_invalidAddr.txt",
                )
            ],
        )
    )
    testList.append(
        TestFeature(
            name="Self update success",
            scenario=[
                TestScenario(
                    cmd="DataMemoryLoad", input="FW_1.txt", expected="SelfUpdate_OK.txt"
                )
            ],
        )
    )
    testList.append(
        TestFeature(
            name="Self update CRC error",
            scenario=[
                TestScenario(
                    cmd="DataMemoryLoad",
                    input="FW_crcError.txt",
                    expected="SelfUpdate_crcError.txt",
                )
            ],
        )
    )
    testList.append(
        TestFeature(
            name="Backup success",
            scenario=[
                TestScenario(
                    cmd="BackupMemoryLoad", input="FW_1.txt", expected="Backup_OK.txt"
                )
            ],
        )
    )

    approved = True
    for test in testList:
        sys.stdout.flush()
        if at.Execute(test) == False:
            approved = False
            break

    sys.stdout.flush()

    if not approved:
        os._exit(1)


class AutoTest:
    def __init__(self, binName, inDir, outDir, expectDir):
        self.binName = binName
        self.inDir = inDir
        self.outDir = outDir
        self.expectDir = expectDir

        subprocess.call(["rm", "-rf", outDir + "*"], stdout=subprocess.DEVNULL)

        self.index = 0

    def Execute(self, testFeature, show_diff=False) -> bool:
        self.index += 1

        args = [self.binName]

        for test in testFeature.scenario:
            test.input = self.inDir + test.input
            test.output = self.outDir + test.output
            test.expected = self.expectDir + test.expected
            args.append(test.cmd)
            args.append(test.input)
            args.append(test.output)

        subprocess.call(args, stdout=subprocess.DEVNULL)

        result = True
        for test in testFeature.scenario:
            result = False
            if not os.path.exists(test.expected):
                logging.info(f"{test.expected} Not found")
            elif not os.path.exists(test.output):
                logging.info(f"{test.output} Not found")
            else:
                if filecmp.cmp(test.expected, test.output, shallow=False):
                    result = True
                else:
                    logging.info(
                        f"Comparison between {test.output} and {test.expected} Unmatched"
                    )
                    if show_diff:
                        diff_tool = "meld" if which("meld") is not None else "diff"
                        subprocess.call(
                            [
                                diff_tool,
                                os.path.abspath(test.output),
                                os.path.abspath(test.expected),
                            ],
                            stdout=subprocess.DEVNULL,
                        )
            if result != True:
                logging.info("Failed to run: " + " ".join(args))

        logging.info(
            "| Test case {}: {: <59} {:>8} |".format(
                self.index, testFeature.name, "Approved" if result else "Failed"
            )
        )
        logging.info("+{:->84}".format("+"))
        return result


class TestScenario:
    def __init__(self, cmd, input, expected, output="result"):
        self.cmd = cmd
        self.input = input
        self.output = output + "_" + expected
        self.expected = expected


class TestFeature:
    def __init__(self, name, scenario):
        self.name = name
        self.scenario = scenario


if __name__ == "__main__":
    args = parser.parse_args()
    logging.basicConfig(level=args.logging.upper())
    sys.exit(main(args))
