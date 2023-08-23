use std::path::Path;

fn main() {
    println!("cargo:rerun-if-changed=src/lib.rs");
    println!("cargo:rerun-if-changed=include/wrapper.hpp");
    println!("cargo:rerun-if-changed=src/wrapper.cpp");

    let sdk_path = Path::new(env!("AMDRMMONITORSDKPATH"));
    let inc_path = sdk_path.join("include");
    let lib_path = sdk_path.join("lib");

    println!("cargo:rustc-link-lib=dylib=Shell32");
    println!("cargo:rustc-link-lib=dylib=Netapi32");
    println!(
        "cargo:rustc-link-search=native={}",
        lib_path.to_str().unwrap()
    );
    println!("cargo:rustc-link-lib=static=Platform");

    cxx_build::bridge("src/lib.rs")
        .file("src/wrapper.cpp")
        .include(inc_path)
        .link_lib_modifier("+bundle")
        .compile("jahe");
}
