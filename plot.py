#!/usr/bin/env python
# coding: utf-8

# In[2]:


import pandas as pd
import seaborn as sns
import numpy as np
import matplotlib.pyplot as plt


# In[3]:


df=pd.read_csv("plot.txt",names=['Y'])


# In[4]:


temp=np.array(df['Y'])


# In[5]:


sns.set()


# In[6]:


bcast=[]
reduce=[]
gather=[]
for i in range(0,len(temp),6):
    bcast.append(temp[i])
    bcast.append(temp[i+1])
    reduce.append(temp[i+2])
    reduce.append(temp[i+3])
    gather.append(temp[i+4])
    gather.append(temp[i+5])


# In[7]:



def plot_fcn(y,index):
    i=0
    plt.figure(figsize=(12,12), dpi=80)
    demo_input_format = pd.DataFrame.from_dict({
    "D": [],
    "P": [],
    "ppn": [],
    "mode": [],  # 1 --> optimized, 0 --> standard
    "time": [],
    })
    for execution in range(10):
        for P in [4, 16]:
            for ppn in [1, 8]:
                for D in [16, 256, 2048]:
                    # Change with the actual data
                    demo_input_format = demo_input_format.append({
                        "D": D, "P": P, "ppn": ppn, "mode": 0, "time": y[i]
                    }, ignore_index=True)
                    demo_input_format = demo_input_format.append({
                        "D": D, "P": P, "ppn": ppn, "mode": 1, "time": y[i+1]
                    }, ignore_index=True)
                    i+=2
    demo_input_format["(P, ppn)"] = list(map(lambda x, y: ("(" + x + ", " + y + ")"), map(str, demo_input_format["P"]), map(str, demo_input_format["ppn"])))
    sns.catplot(x="(P, ppn)", y="time", data=demo_input_format, kind="box", col="D", hue="mode")
    if index==1:
        plt.savefig("plot_bcast.jpg")
    if index==2:
        plt.savefig("plot_reduce.jpg")
    if index==3:
        plt.savefig("plot_gather.jpg")


# In[8]:


plot_fcn(bcast,1)


# In[9]:


plot_fcn(reduce,2)


# In[10]:


plot_fcn(gather,3)


# In[ ]:





# In[ ]:





# In[ ]:





# In[ ]:





# In[ ]:




