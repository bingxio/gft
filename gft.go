//
//        gft: A Go language tab formatting tool.
//             Format a file or all .go files in a directory.
//
//        > usage: gft <file | path>
//
//                GPL 3.0 License
//
package main

import (
    "flag"
    "fmt"
    "io/fs"
    "io/ioutil"
    "os"
    "path/filepath"
    "strings"
)

var (
    chars     []rune // Rune slices of file
    positions []int  // Where the TAB appears
    path      string // Path of the file currently being process
)

const (
    FILE_ERR   = "file: please appoint a .go file or dir"
    OPEN_ERR   = "file: can not open this file or dir"
    CREATE_ERR = "file: can not create formatted file"
)

// Load a file and convert it to a rune slice
func loadFile(p string) *[]string {
    f, err := os.Open(p)
    if err != nil {
        panic(fmt.Sprintf("%s %s\n", OPEN_ERR, err.Error()))
    }
    i, _ := f.Stat()
    // Directory
    if i.IsDir() {
        x := new([]string)
        *x = loadDirFiles(i.Name())
        return x
    } else {
        // File mode
        if p == "" || !strings.HasSuffix(p, ".go") {
            panic(FILE_ERR)
        }
        b, err := ioutil.ReadFile(p)
        if err != nil {
            panic("file: " + err.Error())
        }
        chars = []rune(string(b)) // Runes
        path = p                  // File name
    }
    return nil
}

// Load all .go files in the directory,
// including subdirectories
func loadDirFiles(path string) []string {
    var files []string
    filepath.Walk(path, func(path string, info fs.FileInfo, err error) error {
        if err != nil {
            return err
        } else {
            // All .go files are appended
            if strings.HasSuffix(info.Name(), ".go") {
                files = append(files, path)
            }
            return nil
        }
    })
    return files
}

// Find all TAB positions in rune slice
func lexIdent() {
    for i := 0; i < len(chars); i++ {
        if chars[i] == '\t' {
            positions = append(positions, i)
        }
    }
}

// Write the new file with the same name as the original file
func writeFile() {
    f, err := os.Create(path)
    if err != nil {
        panic(fmt.Sprintf("%s %s\n", CREATE_ERR, err.Error()))
    }
    // Write
    for i := 0; i < len(chars); i++ {
        if contain(i, positions) {
            f.Write([]byte(" "))
            f.Write([]byte(" "))
            f.Write([]byte(" "))
            f.Write([]byte(" "))
        } else {
            f.Write([]byte(string(chars[i])))
        }
    }
    // Prompt message
    fmt.Println("OK: ", path)
}

// Entrance
func main() {
    flag.Parse()
    arg := flag.Args()

    if len(arg) < 1 {
        panic(FILE_ERR)
    } else {
        if ele := loadFile(arg[0]); ele != nil {
            // Dir
            for _, v := range *ele {
                loadFile(v)
                lexIdent()
                writeFile()

                // Clear cache
                chars = make([]rune, 0)
                positions = make([]int, 0)
            }
        } else {
            // File
            lexIdent()
            writeFile()
        }
    }
}

// The location contain the current processing location
func contain(v int, ele []int) bool {
    for _, i := range positions {
        if i == v {
            return true
        }
    }
    return false
}
