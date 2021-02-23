import numpy as np
import numpy.random as r
import time
import random

class Wheel:
    name = 'WM'
    ep = 0.0  # privacy budget epsilon

    d = 0  # domain size + maximum subset size
    m = 0  # maximum subset size
    p = 0.0  # coverage parameter
    trate = 0.0  # hit rate when true
    frate = 0.0  # hit rate when false
    normalizer = 0.0  # normalizer for proportional probabilities
    # seedDist={}
    urlist=[]

    # records for consuming time
    clienttime = 0.0
    recordtime = 0.0
    servertime = 0.0
    # randomtime=0.0
    # covertime=0.0

    def __init__(self, d, m, ep, p=None):
        self.ep = ep
        self.d = d
        self.m = m
        self.p = p

        # records for consuming time
        self.clienttime = 0.0
        self.recordtime = 0.0
        self.servertime = 0.0
        # self.randomtime=0.0
        # self.covertime=0.0
        self.__setparams()

    def __setparams(self):
        if self.p == None:
            self.p = self.bestCoverage(self.d, self.m, self.ep)
        self.normalizer = self.m * self.p * np.exp(self.ep) + (1 - self.m * self.p) * 1.0
        self.trate = self.p * np.exp(self.ep) / self.normalizer
        self.frate = self.p

    @staticmethod
    def bestCoverage(d, m, ep):
        p = 1.0 / (m * np.exp(ep) + 2 * m - 1)
        return p

    def coreRandomizer(self, xs, seed=None):
        z = 0.0

        if seed == None:
            # r.seed(None)
            seed=random.randint(0,100000)*200
        # if int(seed) in self.seedDist:
        #     self.seedDist[int(seed)]+=1
        # else:
        #     self.seedDist[int(seed)]=1
        bs = int(np.ceil(1 / self.p))
        left = [0.0] * bs
        right = [0.0] * bs
        for b in range(0, bs):
            left[b] = min((b + 1) * self.p, 1.0)
            right[b] = b * self.p
        for x in xs:
            r.seed(seed + x)
            v = r.random()
            right[0] = max(v + self.p - 1.0, right[0])
            b = int(np.ceil(v / self.p)) - 1
            left[b] = min(v, left[b])
            if b + 1 < bs:
                right[b + 1] = max(v + self.p, right[b + 1])

        rightleast = right[0]
        for b in range(0, bs - 1):
            left[b] = max(left[b], right[b])
            right[b] = right[b + 1]
        b = bs - 1
        left[b] = max(left[b], right[b])
        right[b] = rightleast + 1
        # compute union length l
        # print(left)
        # print(right)
        # print(np.array(right) - np.array(left))
        l = np.sum(np.array(right) - np.array(left))
        # print(self.p, l, self.m*self.p)
        # ur = self.urlist.pop()


        ur = random.random()
        a = 0.0
        for b in range(0, bs):
            a += np.exp(self.ep) * (right[b] - left[b]) / self.normalizer
            if a > ur:
                z = right[b] - (a - ur) * self.normalizer / np.exp(self.ep)
                break
            a += (self.normalizer - l * np.exp(self.ep)) * (
                        left[(b + 1) % bs] + np.floor((b + 1) * self.p) - right[b]) / ((1.0 - l) * self.normalizer)
            if a > ur:
                z = left[(b + 1) % bs] - (1.0 - l) * self.normalizer * (a - ur) / (
                            self.normalizer - l * np.exp(self.ep))
                break
        return (z, seed)

    def randomizer(self, secrets, seed=None):
        xs = secrets
        # tstart = time.clock()
        z, seed = self.coreRandomizer(xs, seed)
        # self.clienttime += time.clock() - tstart

        return self.recorder(z % 1.0, seed)

    def recorder(self, z, seed):
        # record a value as a hit map
        # tstart = time.clock()
        pub = np.zeros(self.d, dtype=int)
        for i in range(1, self.d):

            r.seed(seed + i)
            v = r.random()

            if (v <= z and z < v + self.p) or (0 <= z and z < v + self.p - 1.0):
                pub[i] = 1
        # self.servertime += time.clock() - tstart
        return pub

    def decoder(self, hits, n):
        # debias hits but without projecting to simplex
        # print('rates', self.trate, self.frate)
        # tstart = time.clock()
        fs = np.array([(hits[i] - n * self.frate) / (self.trate - self.frate) for i in range(0, self.d)])
        # self.servertime += time.clock() - tstart
        return fs

    def bound(self, n, tfs=None):
        # compute theoretical squared l2-norm error bound
        return (self.m * self.trate * (1.0 - self.trate) + (self.d-self.m) * self.frate * (1 - self.frate)) / (
                    n * (self.trate - self.frate) * (self.trate - self.frate))
    def MSE(self,Noisy_Vaule_List,True_Value_List,N):
        result=0
        for i in range(1,len(True_Value_List)):
            tmp=abs(True_Value_List[i]/N-Noisy_Vaule_List[i]/N)
            result+=tmp*tmp
        result /=len(True_Value_List)
        return result
    # def outputseed(self):
    #     for i in self.seedDist:
    #         print("key:",i,",values:",self.seedDist[i])
