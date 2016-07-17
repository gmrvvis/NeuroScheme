#include "MainWindow.h"

// OJO PRUEBAS
#include "reps/QGraphicsItemRepresentation.h"
#include "objs/Neuron.h"
#include "RepresentationCreator.h"


MainWindow::MainWindow( QWidget* parent_ )
  : QMainWindow( parent_ )
  , _ui( new Ui::MainWindow )
{

  _canvas = new neuroscheme::Canvas( this );
  this->setCentralWidget( _canvas );


  shift::Objects neurons;
  shift::Representations representations;

  for ( unsigned int i = 0; i < 4; i++ )
  {
    neurons.push_back( new neuroscheme::Neuron(
                         neuroscheme::Neuron::INTERNEURON,
                         neuroscheme::Neuron::EXCITATORY,
                         10.0f, 30.0f, 30.0f, 50.0f ));

    neurons.push_back( new neuroscheme::Neuron(
                         neuroscheme::Neuron::PYRAMIDAL,
                         neuroscheme::Neuron::INHIBITORY,
                         70.0f, 60.0f, 20.0f, 30.0f ));
  }

  neuroscheme::RepresentationCreator repCreator;
  repCreator.create( neurons, representations );

  unsigned int x = 0;
  for ( const auto representation : representations )
  {
    auto graphicsItemRep =
      dynamic_cast< neuroscheme::QGraphicsItemRepresentation* >(
      representation );
    _canvas->scene( ).addItem( graphicsItemRep->item( ));
    graphicsItemRep->item( )->setPos( x, 0 ); x+= 120;
    // graphicsItemRep->item( )->setScale( 10 );
  }
}

MainWindow::~MainWindow( void )
{
  delete _ui;
}
