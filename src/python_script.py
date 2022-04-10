from re import T
import subprocess
from tabnanny import check
import matplotlib.pyplot as plt
from check_results_in_file import check_results, file_rewrite

subprocess.call(["g++", "main.cpp"], shell=True)
#subprocess.call("a.exe ./config.cfg", shell=True)

#p = subprocess.Popen(["a.exe", "./config.cfg"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

#out, err = p.communicate()
#out = out.decode()

num = 0
results = []
number_of_threads = []
min_time = -1
min_nun_of_treads = 0
num_of_checks = 10

filenames = []
for i in  range(num_of_checks):
    filenames.append(("res_file_n_" + i, "res_file_a_" + i))

for i in range(num_of_checks):
    number_of_threads.append(num)
    f = open("../configs/index.cfg", "r")
    str_file = ""
    lines = f.readlines()
    for line in lines:
        if (line != '\n'):
            if (line.split('=')[0] == "indexing_threads"):
                line = "indexing_threads=" + str(num) + "\n"
                num += 2
        str_file += line

    str_file = str_file.strip()
    f.close()
    f = open("../configs/index.cfg", "w")
    f.write(str_file)
    f.close()

    file_rewrite("../res_n.txt", filenames[i][0])
    file_rewrite("../res_a.txt", filenames[i][1])

    p = subprocess.Popen(["a.exe", "../configs/index.cfg"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    out, err = p.communicate()
    out = out.decode()

    

    try:
        one_result_numbers = out.split('\r\n')
        for i in range(4):
            one_result_numbers[i] = int(number_of_threads[i].split("=")[1])
    except ValueError:
        print("Error occured. Something wrong with your output.")
        print(out)
        exit()
    if (min == -1):
        min_time = one_result_numbers[0]
        min_nun_of_treads = num-2
    elif (min > one_result_numbers[0]):
        min_time = one_result_numbers[0]
        min_nun_of_treads = num-2
    
    results.append(one_result_numbers[0])

num_of_errors = []

for i in range(1, num_of_checks):
    num_of_errors.append(
        (f"Errors in files {i}, compared to 0 (err in n, err in a)",
        check_results( filenames[0][0], filenames[i][0]),
        check_results( filenames[0][1], filenames[i][1]))
    )

print("Minimal time: ", min_time)
print("Number of threads for minimal time", min_nun_of_treads)


plt.plot(number_of_threads, results)
plt.xlabel('number of threads')
plt.ylabel('time in microseconds')
plt.title('graph')
plt.show()


