library(shiny)


ui <- fluidPage(sidebarLayout(
  sidebarPanel(
    # # Compile
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
    actionButton("visual", "Visualize"),
    
    textInput(
      inputId = "command",
      label = "",
      value = "ls"
    ),
    actionButton("btn", "run")
  ),
  
  mainPanel(textOutput("cpp"))
))

server <- function(input, output, session) {
  observeEvent(input$btn, {
    output$cpp <- renderText({
      paste(system(input$command))
    })
  })
}

shinyApp(ui, server)