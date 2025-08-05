import os
import platform
import subprocess
import sys
import zipfile
import shutil

root_path = ""
version = ""
os_type = sys.platform

def run_command(command):
    result = subprocess.run(command, shell=True)
    if result.returncode != 0:
        raise Exception(f"Command '{command}' failed with exit code {result.returncode}.")


def check_cloned():
    # Check if the repository has been cloned
    os.chdir(root_path)
    if not os.path.exists("scheduler"):
        return False
    return True


def clone_scheduler():
    print("Cloning scheduler...")
    os.chdir(root_path)
    # Clone or update nlohmann/json using Git (you need Git installed)
    if not os.path.exists("scheduler"):
        run_command("git clone https://github.com/lsmon/scheduler.git scheduler")
    else:
        os.chdir("scheduler")
        run_command("git pull origin main")
        os.chdir("../..")
    print("Cloning completed.")


def check_latest_version():
    # Check if the local repository is up to date
    local_dir = os.path.join(root_path, "scheduler")
    try:
        subprocess.check_call(["git", "remote", "update"], cwd=local_dir)
        subprocess.check_call(["git", "status", "--ahead-behind", "origin/main"], cwd=local_dir)
    except subprocess.CalledProcessError:
        return False
    return True


def update_repository():
    # Update the local repository to the latest version
    local_dir = os.path.join(root_path, "scheduler")
    try:
        subprocess.check_call(["git", "pull", "origin", "main"], cwd=local_dir)
    except subprocess.CalledProcessError:
        print("Error updating repository")


def build_scheduler():
    print("Building scheduler...")

    path = os.path.join(root_path, "scheduler")
    build = os.path.join(path, "build")
    os.chdir(path)
    os.makedirs(build, exist_ok=True)

    os_name = platform.system()
    print(os_name)    
    
    cmake_bin = ""
    compiler_flag = ""
    if os_type == "darwin": 
        cmake_bin = "/opt/homebrew/bin/cmake"
        compiler_flag = "-DCMAKE_C_COMPILER:FILEPATH=/usr/bin/clang -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/clang++"
    else :
        cmake_bin = "cmake"
    
    run_command(cmake_bin + " --no-warn-unused-cli -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE " + compiler_flag + " -S" + path + " -B" + build)
    run_command(cmake_bin + " --build " + build + " --config Debug --target all")
    print("Building completed.")



def cpack_scheduler():
    print("Packing scheduler")
    scheduler_path = os.path.join(root_path, "scheduler")
    scheduler_build = os.path.join(scheduler_path, "build")
    os.chdir(scheduler_build)
    if os_type == "linux":
        cpack = "/usr/bin/cpack"
    elif os_type == "Darwin":
        cpack = "/opt/homebrew/bin/cpack"
    else:
        cpack = "cpack"
    run_command(cpack + " -G ZIP")


def install_scheduler():
    scheduler_path = os.path.join(root_path, "scheduler")
    scheduler_build = os.path.join(scheduler_path, "build")
    os_postfix = ""
    if os_type == "linux":
        os_postfix = "Linux"
    elif os_type == "Windows":
        os_postfix = "Windows"
    else:
        os_postfix = "Darwin"
    scheduler_libzip = os.path.join(scheduler_build, f"lib_scheduler-{version}-{os_postfix}.zip")
    scheduler_libinc = os.path.join(scheduler_build, f"lib_scheduler-{version}-{os_postfix}/include")
    scheduler_lib = os.path.join(scheduler_build, f"lib_scheduler-{version}-{os_postfix}/lib/lib_scheduler-{version}.a")
    lib_path = os.path.join(root_path, "lib")
    inc_path = os.path.join(root_path, "include")

    print(scheduler_libzip)
    with zipfile.ZipFile(scheduler_libzip, 'r') as zip_ref:
        zip_ref.extractall(scheduler_build)

    if not(os.path.exists(lib_path)):
        os.makedirs(lib_path)
    print("copying " + scheduler_lib + " to " + lib_path)
    shutil.copy(scheduler_lib, lib_path)

    if not os.path.exists(inc_path):
        os.makedirs(inc_path)

    src_path = os.path.join(scheduler_libinc)
    dst_path = os.path.join(root_path, "include")
    copy_hpp(src_path, dst_path)


# Iterate over files in netpp_libinc and copy each file to inc_path
def copy_hpp(src_path, dst_path):
    for root, dirs, files in os.walk(src_path):
        for dir in dirs:
            src_dir = os.path.join(root, dir)
            dst_dir = os.path.join(dst_path, os.path.relpath(src_dir, src_path))
            print("Creating dir: " + dst_dir)
            os.makedirs(dst_dir, exist_ok=True)
            # shutil.copytree(src_dir, dst_dir, dirs_exist_ok=True)
        for file in files:
            src_file = os.path.join(root, file)
            dst_file = os.path.join(dst_path, os.path.relpath(src_file, src_path))
            print("Copying " + src_file + " -> " + dst_file)
            shutil.copy2(src_file, dst_file)

def main():
    if not check_cloned():
        clone_scheduler()
    else:
        if not check_latest_version():
            update_repository()
    build_scheduler()
    cpack_scheduler()
    install_scheduler()
    print("All done!")


if __name__ == "__main__":
    num_args = len(sys.argv)
    if num_args < 2:
        print("installation root path is needed")
    else:
        root_path = sys.argv[1]
        version = sys.argv[2]
        print(root_path)
        print(version)
        main()