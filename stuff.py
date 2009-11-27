#!/usr/bin/python

ILLEGAL = ()


class Tree(object):
    def __init__(self, val, left=None, right=None):
        self.val = val
        self.left = left
        self.right = right


def join_is_legal(r1, r2):
    return (r1, r2) not in ILLEGAL


def make_query_tree(rels):
    trees = make_one_level_trees(rels)
    return make_query_tree_from_trees(trees)

def make_query_tree_from_trees(trees):
    
