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
    if not os.path.exists("cache"):
        return False
    return True


def clone_cache():
    print("Cloning cache...")
    os.chdir(root_path)
    # Clone or update nlohmann/json using Git (you need Git installed)
    if not os.path.exists("cache"):
        run_command("git clone https://github.com/lsmon/cache.git cache")
    else:
        os.chdir("cache")
        run_command("git pull origin main")
        os.chdir("../..")
    print("Cloning completed.")


def check_latest_version():
    # Check if the local repository is up to date
    local_dir = os.path.join(root_path, "cache")
    try:
        subprocess.check_call(["git", "remote", "update"], cwd=local_dir)
        subprocess.check_call(["git", "status", "--ahead-behind", "origin/main"], cwd=local_dir)
    except subprocess.CalledProcessError:
        return False
    return True


def update_repository():
    # Update the local repository to the latest version
    local_dir = os.path.join(root_path, "cache")
    try:
        subprocess.check_call(["git", "pull", "origin", "main"], cwd=local_dir)
    except subprocess.CalledProcessError:
        print("Error updating repository")


def build_cache():
    print("Building cache...")

    cache_path = os.path.join(root_path, "cache")
    cache_build = os.path.join(cache_path, "build")
    os.chdir(cache_path)
    os.makedirs(cache_build, exist_ok=True)

    os_name = platform.system()
    print(os_name)
    # run_command("cmake -S " + cache_path + " -B " + cache_build)
    # run_command("cmake --build " + cache_build + " -j 14")
    cmake_bin = "";
    if os_name == "Darwin":
        cmake_bin = "/opt/homebrew/bin/cmake"
    else :
        cmake_bin = "cmake"
    print(cmake_bin + " --no-warn-unused-cli -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -S" + cache_path + " -B" + cache_build)
    print(cmake_bin + " --build " + cache_build + " --config Debug --target all -j 12 --")
    run_command(cmake_bin + " --no-warn-unused-cli -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -S" + cache_path + " -B" + cache_build)
    run_command(cmake_bin + " --build " + cache_build + " --config Debug --target all -j 12 --")

    print("Building completed.")


def cpack_cache():
    print("Packing cache")
    cache_path = os.path.join(root_path, "cache")
    cache_build = os.path.join(cache_path, "build")
    os.chdir(cache_build)
    if os_type == "linux":
        cpack = "/usr/bin/cpack"
    elif os_type == "Darwin":
        cpack = "/opt/homebrew/bin/cpack"
    else:
        cpack = "cpack"
    run_command(cpack + " -G ZIP")


def install_cache():
    cache_path = os.path.join(root_path, "cache")
    cache_build = os.path.join(cache_path, "build")
    os_postfix = ""
    if os_type == "linux":
        os_postfix = "Linux"
    elif os_type == "Windows":
        os_postfix = "Windows"
    else:
        os_postfix = "Darwin"
    cache_libzip = os.path.join(cache_build, f"lib_caching-{version}-{os_postfix}.zip")
    cache_libinc = os.path.join(cache_build, f"lib_caching-{version}-{os_postfix}/include")
    cache_lib = os.path.join(cache_build, f"lib_caching-{version}-{os_postfix}/lib/lib_caching-{version}.a")
    lib_path = os.path.join(root_path, "lib")
    inc_path = os.path.join(root_path, "include")

    print(cache_libzip)
    with zipfile.ZipFile(cache_libzip, 'r') as zip_ref:
        zip_ref.extractall(cache_build)

    if not(os.path.exists(lib_path)):
        os.makedirs(lib_path)
    print("copying " + cache_lib + " to " + lib_path)
    shutil.copy(cache_lib, lib_path)

    if not os.path.exists(inc_path):
        os.makedirs(inc_path)

    src_path = os.path.join(cache_libinc)
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
        clone_cache()
    else:
        if not check_latest_version():
            update_repository()
    build_cache()
    cpack_cache()
    install_cache()
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