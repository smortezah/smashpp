library(shiny)

ui <- fluidPage(sidebarLayout(
  sidebarPanel(
    # Compile
    textInput(
      inputId = "command",
      label = "Compilation",
      value = "g++ -g -O3 -std=c++17 -pthread -Wall ..\\src\\par.cpp 
      ..\\src\\fcm.cpp ..\\src\\tbl64.cpp ..\\src\\tbl32.cpp ..\\src\\cmls4.cpp 
      ..\\src\\logtbl8.cpp ..\\src\\filter.cpp ..\\src\\segment.cpp 
      ..\\src\\color.cpp ..\\src\\svg.cpp ..\\src\\vizpaint.cpp 
      ..\\src\\main.cpp -o smashpp"
    ),
    actionButton("compile", "compile"),
    
    # sliderInput(
    #   "optimize",
    #   "Optimization level",
    #   0,
    #   min = 0,
    #   max = 3
    # ),
    # checkboxInput("debug", "Debug", value = TRUE),
    # selectInput("warning", "Warning level",
    #             c("all"), selected = "all"),
    # fileInput(
    #   "input",
    #   "Source files",
    #   multiple = TRUE,
    #   accept = c("all")
    # ),
    # textInput("output", "Output", value = "smashpp"),
    # actionButton("compile", "Compile"),
    
    # Run
    sliderInput(
      "compress",
      "Compression level",
      3,
      min = 0,
      max = 5
    ),
    actionButton("run", "Run"),
    
    # Visualize
    selectInput("mode", "Mode",
                c("Horizontal", "Vertical"), selected = "Vertical"),
    actionButton("visual", "Visualize")
  ),
  
  mainPanel(verbatimTextOutput("cpp", placeholder = TRUE))
))

server <- function(input, output, session) {
  console_out <- eventReactive(input$compile, {
    capture.output(cat(system(input$command, intern = TRUE), sep = '\n'))
  })
  
  output$cpp <- renderPrint({
    console_out()
  })
}

shinyApp(ui, server)