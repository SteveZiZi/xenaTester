# -*- coding: cp936 -*-

import sys
import os
import xml.dom.minidom


# 获取xml的 版本号 属性值
def GetVersion( xmlfile ):
	# 读取xml文件
	xmldoc = xml.dom.minidom.parse(xmlfile)

	# 找到目标node
	config_nodes = xmldoc.getElementsByTagName("config")
	mainVer_nodes = config_nodes[0].getElementsByTagName("mainVer")
	buildVer_nodes = config_nodes[0].getElementsByTagName("buildVer")
	
	return mainVer_nodes[0].attributes["val"].value + "." +buildVer_nodes[0].attributes["val"].value
	

# 获取xml的 设备名字 属性值
def GetName( xmlfile ):
	# 读取xml文件
	xmldoc = xml.dom.minidom.parse(xmlfile)

	# 找到目标node
	config_nodes = xmldoc.getElementsByTagName("config")
	name_nodes = config_nodes[0].getElementsByTagName("name")
	
	return name_nodes[0].attributes["val"].value
	

# python make_release_dir.py ../../release/tmp  ../bin/version.xml
# python make_release_dir.py 需要更名的文件夹    版本配置文件
if __name__ == "__main__":

	# 获得参数个数
	argc = len(sys.argv)
	
	for i in range(1, argc, 2):
	
		tmp = GetVersion(sys.argv[i+1])
		version = tmp.encode("gbk")
		tmp = GetName(sys.argv[i+1])  
		name = tmp.encode("gbk")  
		if os.path.isfile(sys.argv[i]) == False:        
			# 创建包所在的目录
			setupdir = os.path.dirname(os.path.abspath(sys.argv[i]))+ "\\" + name + "_" + version
			print "setupdir:", setupdir
			if os.path.exists(setupdir) == True:
			   raise AssertionError(setupdir, " has existed!") 
			else:
				os.rename(sys.argv[i], setupdir)  
		else:
			ext = os.path.splitext(sys.argv[i])
			setupdir = os.path.dirname(os.path.abspath(ext[0])) + "\\" + name + "_" + version + ext[1]
			if os.path.exists(setupdir) == True:
			   raise AssertionError(setupdir, " has existed!") 
			else:
				os.rename(sys.argv[i], setupdir) 
					