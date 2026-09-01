let aboutImages = document.querySelectorAll(".about_small_image img");
let full;

aboutImages.forEach((aboutImage) =>{
    aboutImage.addEventListener("click", (e) =>{
        full = document.getElementById("imagebox");
        full.src = e.currentTarget.src;
    })
})