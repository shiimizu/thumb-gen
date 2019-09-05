# Python program to find SHA256 hash string of a file
import sys
import hashlib
import os
import base64

#filename = input("Enter the input file name: ")
sha256_hash = hashlib.sha256()
md5_hash = hashlib.md5()
if len(sys.argv) != 2:
	print("Need one input file")
	sys.exit()
filename = sys.argv[1]
if not os.path.exists(filename):
	sys.exit("File doesn't exist!")
with open(filename,"rb") as f:
    # Read and update hash string value in blocks of 4K
    for byte_block in iter(lambda: f.read(4096),b""):
        sha256_hash.update(byte_block)
        md5_hash.update(byte_block)
    # sha256_hash.hexdigest()
    sha256_base64 = base64.b64encode(sha256_hash.digest()).decode()
    print("SHA256:",sha256_base64)

    #md5_base64 = base64.b64encode(bytes.fromhex(md5_hash.hexdigest()))
    md5_base64 = base64.b64encode(md5_hash.digest()).decode()
    print("MD5:",md5_base64)