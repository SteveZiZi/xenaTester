# -*- coding: cp936 -*-

import sys
import os
import zipfile


# 对文件夹foldername 进行压缩, 压缩后的文件filename
def zipfolder( foldername, filename ):
	'''
		zip folder foldername and all its subfiles and folders into
		a zipfile named filename
	'''
	empty_dirs = []
	zip = zipfile.ZipFile(filename, 'w', zipfile.ZIP_DEFLATED)
	for root,dirs,files in os.walk(foldername):
	
		# 获得所有的空目录
		for dir in dirs:
			if os.listdir(os.path.join(root, dir)) == []:
				empty_dirs.append(os.path.join(root, dir)[len(foldername)+1:])

	
		# 注意, 这里遍历到空的文件夹时, 并不会把文件夹加入zip文件, 如需要, 额外添加对空文件夹的处理代码
		for file in files:
			srcfile = os.path.join(root, file)
			print "compressing", srcfile
			zip.write(srcfile, srcfile[len(foldername):])
	
	# 将空目录作为空文件加入zip
	for dir in empty_dirs:
		zip.writestr(dir+"\\", "")

	zip.close()
	print "Finish compressing:", os.path.abspath(filename)

# python zip_folder.py ..\bin
# python zip_folder.py  需要压缩的文件夹
if __name__ == "__main__":

	# 获得参数个数
	argc = len(sys.argv)  
	if argc == 0:
		raise AssertionError("not set zip folder!") 

	base_name = os.path.basename(os.path.abspath(sys.argv[1])) + ".zip"
	dst_path = sys.argv[1] + "\\..\\" + base_name
	if os.path.exists(dst_path) == True:
		raise TypeError(dst_path, " file has existed!")
	else:              
		zipfolder(sys.argv[1], dst_path);
		