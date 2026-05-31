#!/usr/bin/env python3
"""
Простой генератор тестового .torrent файла для отладки
"""
import hashlib
import time

def bencode_string(s):
	"""Кодирует строку в bencode формат"""
	if isinstance(s, str):
		s = s.encode('utf-8')
	return f"{len(s)}:{s.decode('utf-8')}".encode('utf-8')

def bencode_int(i):
	"""Кодирует число в bencode формат"""
	return f"i{i}e".encode('utf-8')

def bencode_dict(d):
	"""Кодирует словарь в bencode формат"""
	result = b"d"
	for key in sorted(d.keys()):
		result += bencode_string(key)
		value = d[key]
		if isinstance(value, str):
			result += bencode_string(value)
		elif isinstance(value, int):
			result += bencode_int(value)
		elif isinstance(value, dict):
			result += bencode_dict(value)
	result += b"e"
	return result

# Создаем простой тестовый торрент
test_data = b"Hello, BitTorrent World!"
piece_hash = hashlib.sha1(test_data).digest()

info = {
	"name": "test.txt",
	"length": len(test_data),
	"piece length": 16384,
	"pieces": piece_hash
}

torrent = {
	"announce": "http://tracker.example.com/announce",
	"info": info,
	"created by": "test_creator/1.0",
	"creation date": int(time.time())
}

# Сохраняем торрент-файл
with open("test_file.torrent", "wb") as f:
	f.write(bencode_dict(torrent))

print("Created test_file.torrent")
print(f"File size: {len(test_data)} bytes")
print(f"Piece hash: {piece_hash.hex()}")
