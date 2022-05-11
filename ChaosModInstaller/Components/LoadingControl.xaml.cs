using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace ChaosModInstaller
{
    public partial class LoadingControl : UserControl
    {
        public LoadingControl()
        {
            InitializeComponent();
        }

        public int Diameter
        {
            get { return (int)GetValue(DiameterProperty); }
            set
            {
                if (value < 10)
                    value = 10;
                SetValue(DiameterProperty, value);
            }
        }
        public static readonly DependencyProperty DiameterProperty =
            DependencyProperty.Register("Diameter", typeof(int), typeof(LoadingControl), new PropertyMetadata(20, OnDiameterPropertyChanged));

        private static void OnDiameterPropertyChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            var vm = (LoadingControl)d;
            d.CoerceValue(CenterProperty);
            d.CoerceValue(RadiusProperty);
            d.CoerceValue(InnerRadiusProperty);
        }

        public int Radius
        {
            get { return (int)GetValue(RadiusProperty); }
            set { SetValue(RadiusProperty, value); }
        }
        public static readonly DependencyProperty RadiusProperty =
            DependencyProperty.Register("Radius", typeof(int), typeof(LoadingControl), new PropertyMetadata(15, null, OnCoerceRadius));

        private static object OnCoerceRadius(DependencyObject d, object baseValue)
        {
            var control = (LoadingControl)d;
            int newRadius = (int)(control.GetValue(DiameterProperty)) / 2;
            return newRadius;
        }

        public int InnerRadius
        {
            get { return (int)GetValue(InnerRadiusProperty); }
            set { SetValue(InnerRadiusProperty, value); }
        }
        public static readonly DependencyProperty InnerRadiusProperty =
            DependencyProperty.Register("InnerRadius", typeof(int), typeof(LoadingControl), new PropertyMetadata(2, null, OnCoerceInnerRadius));

        private static object OnCoerceInnerRadius(DependencyObject d, object baseValue)
        {
            var control = (LoadingControl)d;
            int newInnerRadius = (int)(control.GetValue(DiameterProperty)) / 4;
            return newInnerRadius;
        }

        public Point Center
        {
            get { return (Point)GetValue(CenterProperty); }
            set { SetValue(CenterProperty, value); }
        }
        public static readonly DependencyProperty CenterProperty =
            DependencyProperty.Register("Center", typeof(Point), typeof(LoadingControl), new PropertyMetadata(new Point(15, 15), null, OnCoerceCenter));

        private static object OnCoerceCenter(DependencyObject d, object baseValue)
        {
            var control = (LoadingControl)d;
            int newCenter = (int)(control.GetValue(DiameterProperty)) / 2;
            return new Point(newCenter, newCenter);
        }

        public Color Color1
        {
            get { return (Color)GetValue(Color1Property); }
            set { SetValue(Color1Property, value); }
        }
        public static readonly DependencyProperty Color1Property =
            DependencyProperty.Register("Color1", typeof(Color), typeof(LoadingControl), new PropertyMetadata(Colors.Green));

        public Color Color2
        {
            get { return (Color)GetValue(Color2Property); }
            set { SetValue(Color2Property, value); }
        }
        public static readonly DependencyProperty Color2Property =
            DependencyProperty.Register("Color2", typeof(Color), typeof(LoadingControl), new PropertyMetadata(Colors.Transparent));
    }
}
