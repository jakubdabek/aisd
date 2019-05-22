from pathlib import Path
import sys
import itertools
import re
from collections import defaultdict

if (len(sys.argv) < 2):
    exit()

filepath = Path("./out") / sys.argv[1]

results = defaultdict(lambda: defaultdict(dict))

class Result:
    def __init__(self, lines):
        self.data_type, name = lines[0][:-1].split('/')
        self.data_name, self.tree_type = name[:-4].rsplit('-', maxsplit=1)
        self.time = lines[1][lines[1].find(':') + 2:]
        self.comparisons = lines[6][lines[6].find(':') + 2:]
        self.modifications = lines[7][lines[7].find(':') + 2:]
        tmp = re.findall(r'\(([^)]*)\)', lines[8])
        _, self.total_time_insert, self.total_comparisons_insert, self.total_modifications_insert = \
            tmp[0].split(', ')
        _, self.total_time_search, self.total_comparisons_search, self.total_modifications_search = \
            tmp[1].split(', ')
        _, self.total_time_delete, self.total_comparisons_delete, self.total_modifications_delete = \
            tmp[2].split(', ')

        tmp = re.findall(r'\(([^)]*)\)', lines[9])
        _, self.partial_time_insert, self.partial_comparisons_insert, self.partial_modifications_insert = \
            tmp[0].split(', ')
        _, self.partial_time_search, self.partial_comparisons_search, self.partial_modifications_search = \
            tmp[1].split(', ')
        _, self.partial_time_delete, self.partial_comparisons_delete, self.partial_modifications_delete = \
            tmp[2].split(', ')
        
        self.max_size = lines[10][lines[10].find(':') + 2:]
        

with filepath.open(newline='') as file:
    while True:
        result = list(s.strip() for s in itertools.islice(file, 13))
        if not result:
            break
        # print(result)
        result = Result(result)
        results[result.data_type][result.data_name][result.tree_type] = result

def print_formatted():
    for data_type, names in results.items():
        for data_name, trees in names.items():
            print(data_type, data_name)
            if 'bst' in trees:
                bst = trees['bst']
            else:
                bst = None
            rbt = trees['rbt']
            splay = trees['splay']
            if bst:
                print(bst.partial_time_insert, rbt.partial_time_insert, splay.partial_time_insert)
                print(bst.partial_comparisons_insert, rbt.partial_comparisons_insert, splay.partial_comparisons_insert)
                print(bst.partial_modifications_insert, rbt.partial_modifications_insert, splay.partial_modifications_insert)
                print(bst.partial_time_search, rbt.partial_time_search, splay.partial_time_search)
                print(bst.partial_comparisons_search, rbt.partial_comparisons_search, splay.partial_comparisons_search)
                print(bst.partial_modifications_search, rbt.partial_modifications_search, splay.partial_modifications_search)
                print(bst.partial_time_delete, rbt.partial_time_delete, splay.partial_time_delete)
                print(bst.partial_comparisons_delete, rbt.partial_comparisons_delete, splay.partial_comparisons_delete)
                print(bst.partial_modifications_delete, rbt.partial_modifications_delete, splay.partial_modifications_delete)
            else:
                print(rbt.partial_time_insert, splay.partial_time_insert)
                print(rbt.partial_comparisons_insert, splay.partial_comparisons_insert)
                print(rbt.partial_modifications_insert, splay.partial_modifications_insert)
                print(rbt.partial_time_search, splay.partial_time_search)
                print(rbt.partial_comparisons_search, splay.partial_comparisons_search)
                print(rbt.partial_modifications_search, splay.partial_modifications_search)
                print(rbt.partial_time_delete, splay.partial_time_delete)
                print(rbt.partial_comparisons_delete, splay.partial_comparisons_delete)
                print(rbt.partial_modifications_delete, splay.partial_modifications_delete)

from pprint import pprint
pprint({k1: {k2: {k3: v3.max_size for k3, v3 in v2.items()} for k2, v2 in v1.items()} for k1, v1 in results.items() if k1 == 'nonunique-sorted'})
pprint({k1: {k2: {k3: v3.partial_time_insert for k3, v3 in v2.items()} for k2, v2 in v1.items()} for k1, v1 in results.items() if k1 == 'nonunique-sorted'})
pprint({k1: {k2: {k3: v3.partial_time_delete for k3, v3 in v2.items()} for k2, v2 in v1.items()} for k1, v1 in results.items() if k1 == 'nonunique-sorted'})