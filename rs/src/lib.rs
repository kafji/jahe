#[cxx::bridge]
mod ffi {
    unsafe extern "C++" {
        include!("jahe/include/wrapper.hpp");

        type BlobstoreClient;

        fn new_blobstore_client() -> UniquePtr<BlobstoreClient>;

        fn is_user_an_admin() -> bool;
        fn is_supported_os() -> bool;
        fn authentic_amd() -> bool;
        fn query_drv_service() -> i32;
        fn is_supported_processor() -> bool;

        fn api_call();
    }
}

pub fn run() {
    if !ffi::is_user_an_admin() {
        println!("user is not admin");
    }

    if !ffi::is_supported_os() {
        println!("os not supported");
    }

    if !ffi::authentic_amd() {
        println!("no AMD processor is found");
    }

    if ffi::query_drv_service() < 0 {
        println!("driver not found");
    }

    if !ffi::is_supported_processor() {
        println!("processor not supported");
    }

    ffi::api_call();
}
