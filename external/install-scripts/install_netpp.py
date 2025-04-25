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
    if not os.path.exists("netpp"):
        return False
    return True

def clone():
    print("Cloning netpp...")
    os.chdir(root_path)
    # Clone or update nlohmann/json using Git (you need Git installed)
    if not os.path.exists("netpp"):
        run_command("git clone https://github.com/lsmon/netpp.git netpp")
    else:
        os.chdir("netpp")
        run_command("git pull origin main")
        os.chdir("..")
    print("Cloning completed.")


def check_latest_version():
    # Check if the local repository is up to date
    local_dir = os.path.join(root_path, "netpp")
    try:
        subprocess.check_call(["git", "remote", "update"], cwd=local_dir)
        subprocess.check_call(["git", "status", "--ahead-behind", "origin/main"], cwd=local_dir)
    except subprocess.CalledProcessError:
        return False
    return True


def update_repository():
    # Update the local repository to the latest version
    local_dir = os.path.join(root_path, "netpp")
    try:
        subprocess.check_call(["git", "pull", "origin", "main"], cwd=local_dir)
    except subprocess.CalledProcessError:
        print("Error updating repository")

def build():
    print("Building netpp...")

    path = os.path.join(root_path, "netpp")
    build_path = os.path.join(path, "build")
    os.chdir(path)
    os.makedirs(build_path, exist_ok=True)

    os_name = platform.system()
    print(os_name)
    # run_command("cmake -S " + path + " -B " + build_path)
    # run_command("cmake --build " + build_path + " -j 14")
    cmake_bin = "";
    if os_name == "Darwin": 
        cmake_bin = "/opt/homebrew/bin/cmake"
    else :
        cmake_bin = "cmake"
    print(cmake_bin + " --no-warn-unused-cli -DBUILD_TEST=OFF -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -S" + path + " -B" + build_path)
    print(cmake_bin + " --build " + build_path + " --config Debug --target all -j 12 --")
    run_command(cmake_bin + " --no-warn-unused-cli -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -S" + path + " -B" + build_path)
    run_command(cmake_bin + " --build " + build_path + " --config Debug --target all -j 12")

    print("Building completed.")


def cpack():
    print("Packing netpp")
    path = os.path.join(root_path, "netpp")
    build = os.path.join(path, "build")
    os.chdir(build)
    run_command("cpack -G ZIP")


def copy_files_and_dirs(src, dst):
    if not os.path.exists(dst):
        os.makedirs(dst)
    for filename in os.listdir(src):
        src_file = os.path.join(src, filename)
        dst_file = os.path.join(dst, filename)
        if os.path.isfile(src_file):
            print(f"copying {src_file} to {dst_file}")
            shutil.copy(src_file, dst_file)
        elif os.path.isdir(src_file):
            copy_files_and_dirs(src_file, dst_file)


def install():
    path = os.path.join(root_path, "netpp")
    build = os.path.join(path, "build")
    os_postfix = ""
    print("os_type: " + os_type)
    if os_type == "linux":
        os_postfix = "Linux"
    elif os_type == "darwin":
        os_postfix = "Darwin"
    
    libzip = os.path.join(build, f"lib_netpp-{version}-{os_postfix}.zip")
    libinc = os.path.join(build, f"lib_netpp-{version}-{os_postfix}/include")
    lib = os.path.join(build, f"lib_netpp-{version}-{os_postfix}/lib/lib_netpp-{version}.a")
    lib_path = os.path.join(root_path, "lib")
    inc_path = os.path.join(root_path, "include")

    print("Extracting " + libzip + " to " + build)
    with zipfile.ZipFile(libzip, 'r') as zip_ref:
        zip_ref.extractall(build)
    
    if not(os.path.exists(lib_path)):
        os.makedirs(lib_path)
    print("copying " + lib + " to " + lib_path)
    shutil.copy(lib, lib_path)

    if not os.path.exists(inc_path):
        os.makedirs(inc_path)
        
    # Iterate over files in libinc and copy each file to inc_path
    print(libinc)
    copy_files_and_dirs(libinc, inc_path)
    # for filename in os.listdir(libinc):
    #     src_file = os.path.join(libinc, filename)
    #     if os.path.isfile(src_file):  # Only copy files (not directories)
    #         print("copying " + src_file + " to " + inc_path)
    #         shutil.copy(src_file, inc_path)
    #     elif os.path.isdir(src_file):
            # recurse this function


def main():
    if not check_cloned():
        clone()
    else:
        if not check_latest_version():
            update_repository()
    build()
    cpack()
    install()
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
