{
  "targets": [
    {
      "target_name": "naaa",
      "sources": [
        "src/naaa.cc",
        "src/Analyser.cc",
        "src/FFTFrame.cc"
      ],
      "conditions": [
        ['OS=="mac"', {
          "include_dirs": [
            "/usr/local/include"
          ],
          "libraries" : [
            "-lavcodec",
            "-lavutil"
          ]
        }],
        ['OS=="linux"', {
          "libraries" : [
            "-lavcodec",
            "-lavutil",
          ]
        }],
        ['OS=="win"', {
          "include_dirs": [
            "$(LIBAV_PATH)include"
            ],
          "libraries" : [
            "-l$(LIBAV_PATH)avcodec",
            "-l$(LIBAV_PATH)avutil"
          ]
        }]
      ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ]
    }
  ]
}
