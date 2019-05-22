import shlex
import sys
import os
import subprocess
from pathlib import Path

root = Path("./data/data")

bst_types = ("bst", "rbt", "splay")

operations = Path("./data/operations/operations.txt").read_text()
if (len(sys.argv) > 1):
    suffix = sys.argv[1]
else:
    suffix = ""

with Path("./out/output{}.txt".format(suffix)).open("w") as cumulative_output:
    for folder in root.iterdir():
        folder = Path(folder)    
        print(folder.name)
        out_dir = Path("./out") / folder.name
        out_dir.mkdir(parents=True, exist_ok=True)
        for file in folder.iterdir():
            print(file.name)
            file = Path(file)
            for t in bst_types:
                if "sorted" in str(file) and ("aspell" in str(file) or "KJB" in str(file)) and t == "bst":
                    continue
                out_file = Path(out_dir / (file.stem + "-" + t + file.suffix))
                print(out_file)
                current_ops = operations.format(file.name)
                # print(current_ops)
                try:
                    subprocess.Popen(
                        ["../../../main", "--type", t],
                        universal_newlines=True,
                        stdin=subprocess.PIPE,
                        stdout=out_file.open("w+"),
                        cwd=str(folder.absolute())
                    ).communicate(input=current_ops)
                except KeyboardInterrupt:
                    continue

                cumulative_output.write(
                    str(out_file.relative_to("./out")) + ":\n" +
                    subprocess.check_output(["tail", "-n", "11", str(out_file.absolute())], universal_newlines=True) +
                    "\n"
                )
                

