import os
import random
import string
import argparse
from tqdm import tqdm

def generate_words_pool(unique_count, min_len=3, max_len=10):
    words = set()
    while len(words) < unique_count:
        word = ''.join(random.choices(string.ascii_lowercase, k=random.randint(min_len, max_len)))
        words.add(word)
    return list(words)

def generate_file_from_pool(filename, target_bytes, unique_count):
    print(f"Generating file {filename} with target size ~{target_bytes // (1 << 20)} MiB and {unique_count} unique words...")
    word_pool = generate_words_pool(unique_count)
    written = 0

    with open(filename, 'w') as f:
        pbar = tqdm(total=target_bytes, unit='B', unit_scale=True)
        while written < target_bytes:
            word = random.choice(word_pool)
            entry = word + ' '
            f.write(entry)
            written += len(entry)
            pbar.update(len(entry))
        pbar.close()

def case_1(filename):
    print("Case 1: 1M total words from 1,000 unique words")
    unique_words = [f"word{i}" for i in range(1000)]
    total_words = 1_000_000
    with open(filename, 'w') as f:
        for _ in tqdm(range(total_words)):
            f.write(random.choice(unique_words) + ' ')

def case_2(filename):
    # ~1 GiB, 100K unique
    generate_file_from_pool(filename, target_bytes=1 << 30, unique_count=100_000)

def case_3(filename):
    # ~10 GiB, 1M unique
    generate_file_from_pool(filename, target_bytes=10 * (1 << 30), unique_count=1_000_000)

def case_4(filename):
    # ~20 GiB, reuse 2M pool
    generate_file_from_pool(filename, target_bytes=20 * (1 << 30), unique_count=2_000_000)

def case_5(filename):
    # ~32 GiB, reuse 5M pool
    generate_file_from_pool(filename, target_bytes=32 * (1 << 30), unique_count=5_000_000)

def main():
    parser = argparse.ArgumentParser(description="Generate test input files for the word counter.")
    parser.add_argument('--case', type=int, required=True, choices=[1, 2, 3, 4, 5],
                        help='1: 1000 unique, 1M total; 2: ~1GB/100K uniq; 3: ~10GB/1M uniq; 4: ~20GB/2M uniq; 5: ~32GB/5M uniq')
    parser.add_argument('--output', type=str, default='input.txt',
                        help='Output filename (default: input.txt)')

    args = parser.parse_args()

    if args.case == 1:
        case_1(args.output)
    elif args.case == 2:
        case_2(args.output)
    elif args.case == 3:
        case_3(args.output)
    elif args.case == 4:
        case_4(args.output)
    elif args.case == 5:
        case_5(args.output)

if __name__ == "__main__":
    main()
