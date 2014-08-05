from PIL import Image
import sys

lowValue = 0 #0 or 255


def main():

	if len(sys.argv) == 1:
		printWithImageNamed("0.bmp")
		print ""
		printWithImageNamed("1.bmp")
		print ""
		printWithImageNamed("2.bmp")
		print ""
		printWithImageNamed("3.bmp")
		print ""
		printWithImageNamed("4.bmp")
		print ""
		printWithImageNamed("5.bmp")
		print ""
		printWithImageNamed("6.bmp")
		print ""
		printWithImageNamed("7.bmp")
		print ""
		printWithImageNamed("8.bmp")
		print ""
		printWithImageNamed("9.bmp")
	elif len(sys.argv) == 2:
		printWithImageNamed(sys.argv[-1])









def printWithImageNamed(imageName):
	im = Image.open(imageName)

	width = im.size[0]
	height = im.size[1]

	#convert to binary
	data = im.convert("1").getdata()

	index = 0;
	for bit in data:
		if index == 0:
			sys.stdout.write('B')
		elif index % 8 == 0:
			sys.stdout.write(', B')
		if bit != lowValue:
			sys.stdout.write('1')
		else:
			sys.stdout.write('0')
		index += 1
		if index == width:
			while index % 8 != 0:
				sys.stdout.write('0')
				index += 1
			sys.stdout.write(',\n')
			index = 0



if __name__ == "__main__":
    main()
