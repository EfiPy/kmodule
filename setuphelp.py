import platform, os, os.path, subprocess

def KmodFolderBuild (Folder):
    if not os.path.isdir (Folder):
        raise NotADirectoryError (f"Folder {Folder} does not exist")

    TopFolder   = os.getcwd()
    Machine     = platform.machine()
    SrcFolder   = os.path.join(TopFolder, Folder)
    BuildFolder = f"{SrcFolder}-{Machine}"

    return TopFolder, SrcFolder, BuildFolder

def KmodConfigureBuild (SrcFolder):

    CurrentDir = os.getcwd ()

    ConfigureFile = os.path.join (SrcFolder, 'configure')
    if os.path.isfile (ConfigureFile):
        print (f'{ConfigureFile} exist.')
        return

    print (f'{ConfigureFile} does not exist.')

    os.chdir (SrcFolder)

    subprocess.run (['touch', 'libkmod/docs/gtk-doc.make'])
    subprocess.run (['autoreconf', '--force', '--install', '--symlink'])

    os.chdir (CurrentDir)

def KmodMakefileBuild (SrcFolder, BuildFolder):

    CurrentDir = os.getcwd ()

    Makefile = os.path.join (BuildFolder, 'Makefile')

    if not os.path.isdir (BuildFolder):
        os.mkdir (BuildFolder)
    elif os.path.isfile (Makefile):
        print (f'{Makefile} exist.')
        return

    print (f'{Makefile} does not exist.')

    os.chdir (BuildFolder)
    RunCmd = os.path.join (SrcFolder, 'configure')

    subprocess.run ([RunCmd, '--disable-manpages', '--disable-test-modules', "CFLAGS=-O2"])

    os.chdir (CurrentDir)

def KmodBuild (BuildFolder):

    CurrentDir = os.getcwd ()
    os.chdir (BuildFolder)

    subprocess.run (['make'])

    os.chdir (CurrentDir)

def KmodClean (BuildFolder):

    CurrentDir = os.getcwd ()
    os.chdir (BuildFolder)

    subprocess.run (['make', 'clean'])

    os.chdir (CurrentDir)

if __name__ == '__main__':
    bf = KmodFolderBuild  ('kmod')
    top, src, build = bf
    print (f"build folder is {bf}")

    KmodConfigureBuild (src)
    KmodMakefileBuild  (src, build)
    KmodBuild (build)
    KmodClean (build)