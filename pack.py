import struct
import sys
import os
import subprocess

def run_command(cmd):
    try:
        # Redirecting errors so we can catch them and show you
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
    
    # --- STEP 1: UNIVERSAL COMPILATION & LINKING ---
    if ext == ".c":
        print(f"[*] Building Universal BPK for {base_name}...")
        obj_file = base_name + ".o"
        bin_file = base_name + ".bin"
        
        # -m32: 32-bit x86 (Universal for old Dells and modern VMs)
        # -ffreestanding: No standard C library (we use Claws instead)
        # -fno-pic / -fno-plt: Fixes the '_GLOBAL_OFFSET_TABLE_' error
        # -fno-stack-protector: Removes extra security code that requires a library
        compile_cmd = (f"gcc -m32 -ffreestanding -fno-pic -fno-plt "
                       f"-fno-stack-protector -c {input_file} -o {obj_file}")
        
        # -Ttext 0: The code starts at address 0 relative to the BPK payload
        # --oformat binary: Strips ALL headers (No ELF, No PE, No Junk)
        link_cmd = f"ld -m elf_i386 -static -Ttext 0 --oformat binary {obj_file} -o {bin_file}"
        
        if not (run_command(compile_cmd) and run_command(link_cmd)):
            return
        
        target_bin = bin_file
    else:
        target_bin = input_file

    # --- STEP 2: BPK WRAPPING ---
    output_bpk = base_name + ".bpk"
    internal_name = os.path.basename(base_name)[:16]
    
    # Header Format: Magic(4 bytes), Name(16 bytes), Version(4 bytes)
    header = struct.pack('4s16sI', b'BPK!', internal_name.encode('utf-8').ljust(16), 1)
    
    try:
        with open(target_bin, 'rb') as f:
            payload = f.read()
            
        with open(output_bpk, 'wb') as f:
            f.write(header)
            f.write(payload)
            
        print(f"[  OK  ] Universal Package: {output_bpk} ({len(payload) + 24} bytes total)")
        
        # Cleanup temp files
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
