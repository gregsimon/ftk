#!/usr/bin/python

# This script downloads, configures, and builds wxWigets.

import os
import subprocess
import errno
import shutil

def install_and_build_wxwidgets():

  if os.name == "nt":
    build_win()
  elif os.name == "osx":
    build_osx()
  elif os.name == "posix":
    build_posix()
  else:
    print("'" + os.name + "' not supported yet")


def build_posix():
  p = subprocess.Popen(['sudo', 'apt-get', 'install', 'libgtk2.0-dev', 'libglw1-mesa', 'libglw1-mesa-dev', 'libglu1-mesa-dev'])
  p.wait()

  make_sure_path_exists("third_party/wxWidgets/build-debug")
  p = subprocess.Popen(['../configure', '-disable-shared', '--enable-monolithic', '--enable-debug', '--with-opengl', '--with-libpng'], \
        cwd=r'third_party/wxWidgets/build-debug/')
  p.wait()
  p = subprocess.Popen(['make', '-j4'], cwd=r'third_party/wxWidgets/build-debug/')
  p.wait()

  make_sure_path_exists("third_party/wxWidgets/build-release")
  p = subprocess.Popen(['../configure', '-disable-shared', '--enable-monolithic', '--with-opengl', '--with-libpng'], \
        cwd=r'third_party/wxWidgets/build-release/')
  p.wait()
  p = subprocess.Popen(['make', '-j4'], cwd=r'third_party/wxWidgets/build-release/')
  p.wait()


def build_win():
  p = subprocess.Popen(['nmake', '/f', 'makefile.vc', 'TARGET_CPU=X64', 'BUILD=release'], \
        cwd=r'third_party/wxWidgets/build/msw/')
  p.wait()

  p = subprocess.Popen(['nmake', '/f', 'makefile.vc', 'TARGET_CPU=X64'], \
        cwd=r'third_party/wxWidgets/build/msw/')
  p.wait()


def build_osx():
  # build debug version
  shutil.rmtree("third_party/wxWidgets/build-debug", True)
  make_sure_path_exists("third_party/wxWidgets/build-debug")
  p = subprocess.Popen(['../configure', '-disable-shared', '--enable-monolithic', '--enable-debug', '--with-libpng'], \
        cwd=r'third_party/wxWidgets/build-debug/')
  p.wait()
  p = subprocess.Popen(['make', '-j4'], cwd=r'third_party/wxWidgets/build-debug/')
  p.wait()

  # build release version
  shutil.rmtree("third_party/wxWidgets/build-release", True)
  make_sure_path_exists("third_party/wxWidgets/build-release")
  p = subprocess.Popen(['../configure', '-disable-shared', '--enable-monolithic', '--with-libpng'], \
        cwd=r'third_party/wxWidgets/build-release/')
  p.wait()
  p = subprocess.Popen(['make', '-j4'], cwd=r'third_party/wxWidgets/build-release/')
  p.wait()


def make_sure_path_exists(path):
    try:
        os.makedirs(path)
    except OSError as exception:
        if exception.errno != errno.EEXIST:
            raise



install_and_build_wxwidgets()
