import os
import numpy as np
import pandas as pd
from enum import  Enum

class TreeState(Enum):
    Alive = 0
    Dead  = 1

class Tree:
    def __init__(self, id, state : TreeState, df : pd.DataFrame):
        self.id = id
        self.state = state
        self.df = df

class SwissTreeParser:
    def __init__(self, path):
        self.path = path
        self.trees  = []

        # Create a list of alive trees
        self.tree_states = []
        for i in range(14):
            self.tree_states.append(TreeState.Alive)

        # Change the position at which we actucally have dead trees
        self.tree_states[0] = TreeState.Dead
        self.tree_states[2] = TreeState.Dead
        self.tree_states[3] = TreeState.Dead
        self.tree_states[6] = TreeState.Dead
        self.tree_states[7] = TreeState.Dead

        for state, i in zip(self.tree_states, range(14)):
            if i < 10:
                df = pd.read_csv(f"{self.path}/Tree_0{i}_State.{state.name}.csv")
            else:
                df = pd.read_csv(f"{self.path}/Tree_{i}_State.{state.name}.csv")
            df['dates'] = pd.to_datetime(df['dates'])
            df.set_index("dates", inplace=True)
            self.trees.append(Tree(id = i, state = state, df = df))
        self.trees = np.array(self.trees)


        self.df_alive_all = pd.DataFrame()
        for tree in self.trees:
            if tree.state == TreeState.Alive:
                self.df_alive_all = pd.concat([self.df_alive_all, tree.df], axis = 0)


