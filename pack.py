import struct
import sys
import os
import subprocess

def run_command(cmd):
    try:
        result = subprocess.run(cmd, check=True, shell=True, capture_output=True, text=True)
        return True
    except subprocess.CalledProcessError as e:
        print(f"[ FAIL ] Build Error:\n{e.stderr}")
        return False

def pack_module(input_file):
    if not os.path.exists(input_file):
        print(f"Error: {input_file} not found.")
        return

    base_name = os.path.splitext(input_file)[0]
    ext = os.path.splitext(input_file)[1]
    

    if ext == ".c":
        print(f"[*] Building Universal BPK for {base_name}...")
        obj_file = base_name + ".o"
        bin_file = base_name + ".bin"
        

        compile_cmd = (f"gcc -m32 -ffreestanding -fno-pic -fno-plt "
                       f"-fno-stack-protector -c {input_file} -o {obj_file}")
        link_cmd = f"ld -m elf_i386 -static -Ttext 0 --oformat binary {obj_file} -o {bin_file}"
        
        if not (run_command(compile_cmd) and run_command(link_cmd)):
            return
        
        target_bin = bin_file
    else:
        target_bin = input_file
    output_bpk = base_name + ".bpk"
    internal_name = os.path.basename(base_name)[:16]
    
    header = struct.pack('4s16sI', b'BPK!', internal_name.encode('utf-8').ljust(16), 1)
    
    try:
        with open(target_bin, 'rb') as f:
            payload = f.read()
            
        with open(output_bpk, 'wb') as f:
            f.write(header)
            f.write(payload)
            
        print(f"[  OK  ] Universal Package: {output_bpk} ({len(payload) + 24} bytes total)")
        
        if ext == ".c":
            if os.path.exists(obj_file): os.remove(obj_file)
            if os.path.exists(bin_file): os.remove(bin_file)
            
    except Exception as e:
        print(f"[ FAIL ] Packing error: {e}")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python3 pack.py <helloworld.c>")
    else:
        pack_module(sys.argv[1])
