import shutil

def check_results(file1, file2):
    f1 = open(file1)
    f2 = open(file2)

    num_of_wrong = 0

    line1 = f1.readline()
    line2 = f2.readline()
    while (not line1 or not line2):
        if (line1 != line2):
            num_of_wrong += 1
        line1 = f1.readline()
        line2 = f2.readline()

    return num_of_wrong

def file_rewrite(original, copy):
    shutil.copyfile(original, copy)